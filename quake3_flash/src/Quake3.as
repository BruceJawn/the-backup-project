package 
{
	import flash.display.Sprite;
	import flash.display.Stage;
	import flash.events.Event;
	import flash.events.KeyboardEvent;
	import flash.events.MouseEvent;
	import flash.events.StatusEvent;
	import flash.net.LocalConnection;
	import flash.ui.Mouse;
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	import flash.utils.getTimer;
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.geom.Rectangle;
	import flash.net.SharedObject;
	import flash.display.StageScaleMode;
	import flash.events.SampleDataEvent;
	import flash.media.SoundChannel;
	import flash.media.Sound;
	import flash.external.ExternalInterface;
	
	import cmodule.quake.CLibInit;
	
	/**
	 * @author Pierre-Yves Gatouillat ==>> gatouillatpy@gmail.com
	 */
	public class Quake3 extends Sprite 
	{
		/**********************************************************************************************************/
		/** ASSETS                                                                                               **/
		/**********************************************************************************************************/
		
		[Embed(source="../embed/demoq3/pak0.pk3", mimeType="application/octet-stream")]
		private var EmbeddedPak0 : Class;
		[Embed(source="../embed/demoq3/pak1.pk3", mimeType="application/octet-stream")]
		private var EmbeddedPak1 : Class;
		[Embed(source="../embed/demoq3/pak2.pk3", mimeType="application/octet-stream")]
		private var EmbeddedPak2 : Class;
		[Embed(source="../embed/demoq3/pak3.pk3", mimeType="application/octet-stream")]
		private var EmbeddedPak3 : Class;
		[Embed(source="../embed/demoq3/pak4.pk3", mimeType="application/octet-stream")]
		private var EmbeddedPak4 : Class;
		[Embed(source="../embed/demoq3/pak5.pk3", mimeType="application/octet-stream")]
		private var EmbeddedPak5 : Class;
		[Embed(source="../embed/demoq3/pak6.pk3", mimeType="application/octet-stream")]
		private var EmbeddedPak6 : Class;
		[Embed(source="../embed/demoq3/pak7.pk3", mimeType="application/octet-stream")]
		private var EmbeddedPak7 : Class;
		[Embed(source="../embed/demoq3/pak8.pk3", mimeType="application/octet-stream")]
		private var EmbeddedPak8 : Class;
		
		/**********************************************************************************************************/
		/** CONSTANTS                                                                                            **/
		/**********************************************************************************************************/
		
		private const GAME : String = "demoq3";
		
		/**********************************************************************************************************/
		/** MEMBERS                                                                                              **/
		/**********************************************************************************************************/
		
		private var _loader : CLibInit;
		private var _swc : Object;
		private var _ram : ByteArray;
		private var _files : Array;
		
		private var _stage : Stage;

		private var _sound : Sound;
		private var _soundChannel : SoundChannel;
		private var _lastSampleDataPosition : int;
		
		private var _renderManager : RenderManager;
		private var _inputManager : InputManager;
		
		private var _q3console : LocalConnection;
		
		private var _last_time : uint;
		
		/**********************************************************************************************************/
		/** PROPERTIES                                                                                           **/
		/**********************************************************************************************************/
		
		public function get swc() : Object
		{
			return _swc;
		}
		
		public function get ram() : ByteArray
		{
			return _ram;
		}
		
		/**********************************************************************************************************/
		/** CONSTRUCTOR                                                                                          **/
		/**********************************************************************************************************/
		
		public function Quake3() : void 
		{
			if ( stage )
				init();
			else
				addEventListener( Event.ADDED_TO_STAGE, init );
		}
		
		/**********************************************************************************************************/
		/** METHODS                                                                                              **/
		/**********************************************************************************************************/
		
		private function init( e : Event = null ) : void 
		{
			removeEventListener( Event.ADDED_TO_STAGE, init );
			addEventListener( Event.REMOVED, exit );
			
			_stage = stage;
			
			TimeManager.init( stage );
			
			_sound = new Sound();
			_sound.addEventListener( SampleDataEvent.SAMPLE_DATA, sampleDataHandler );
			
			_q3console = new LocalConnection();
			_q3console.addEventListener( StatusEvent.STATUS, lcStatusHandler );
			
			_files = new Array();
			_loader = new CLibInit();
			_swc = _loader.init();
			
			_last_time = getTimer();
			
			//fileSupplyDefaultEmbedded( "./demota/q3config.cfg", EmbeddedDefaultConfig );
			
			supplyDir( "./" + GAME + "/" );
			supplyFile( "./" + GAME + "/pak0.pk3", new EmbeddedPak0() as ByteArray );
			supplyFile( "./" + GAME + "/pak1.pk3", new EmbeddedPak1() as ByteArray );
			supplyFile( "./" + GAME + "/pak2.pk3", new EmbeddedPak2() as ByteArray );
			supplyFile( "./" + GAME + "/pak3.pk3", new EmbeddedPak3() as ByteArray );
			supplyFile( "./" + GAME + "/pak4.pk3", new EmbeddedPak4() as ByteArray );
			supplyFile( "./" + GAME + "/pak5.pk3", new EmbeddedPak5() as ByteArray );
			supplyFile( "./" + GAME + "/pak6.pk3", new EmbeddedPak6() as ByteArray );
			supplyFile( "./" + GAME + "/pak7.pk3", new EmbeddedPak7() as ByteArray );
			supplyFile( "./" + GAME + "/pak8.pk3", new EmbeddedPak8() as ByteArray );
			
			_ram = _swc.swcInit( this );
			
			_renderManager = new RenderManager( this );
			_inputManager = new InputManager( this );
			
			_renderManager.onInitComplete.addOnce( onInitComplete );
		}
		
		private function exit( e : Event = null ) : void 
		{
			_stage.removeEventListener( Event.ENTER_FRAME, onFrame );
			
			_inputManager.exit();
			_renderManager.exit();
		}
		
		private function onInitComplete() : void
		{
			_swc.swcInitQuake( _renderManager );
			
			_stage.addEventListener( Event.ENTER_FRAME, onFrame );
		}
		
		public function fileSupplyDefaultEmbedded( filename : String, DefaultEmbed : Class ) : void
		{
			if ( !fileReadSharedObject( filename ) )
			{
				var file : ByteArray = new DefaultEmbed();
				
				_files[filename] = file;
				
				_loader.supplyFile( filename, file );
			}
		}
		
		public function fileReadSharedObject( filename : String ) : Boolean
		{
			var sharedObject : SharedObject = SharedObject.getLocal( filename );
			
			if ( !sharedObject ) return false;
			
			if ( !sharedObject.data.byteArray ) return false;
				
			if ( !_files[filename] )
			{
				var file : ByteArray = sharedObject.data.byteArray;
				
				_files[filename] = file;
				
				_loader.supplyFile( filename, file );
			}
			
			return true;
		}
		
		public function fileWriteSharedObject( filename : String ) : ByteArray
		{
			var sharedObject : SharedObject = SharedObject.getLocal( filename );
			
			if ( !sharedObject ) return undefined;
			
			var file : ByteArray;
			
			if ( !_files[filename] )
			{
				file = new ByteArray;
				
				_files[filename] = file;
				
				_loader.supplyFile( filename, file );
			}
			else
			{
				file = _files[filename];
				
				file.length = 0;
			}
			
			return file;
		}
		
		public function fileUpdateSharedObject( filename : String ) : void
		{
			var sharedObject : SharedObject = SharedObject.getLocal( filename );
			
			if ( !sharedObject ) return;
				
			if ( !_files[filename] )
			{
				trace( "Error: fileUpdateSharedObject() called on a file without a ByteArray" );
			}
			
			sharedObject.data.byteArray = _files[filename];
			sharedObject.flush();
		}
		
		private function supplyDir( path : String ) : void
		{
			_swc.swcRegisterFile( path );
		}
		
		private function supplyFile( path : String, data : ByteArray ) : void
		{
			_loader.supplyFile( path, data );
			
			_swc.swcRegisterFile( path );
		}
		
		public function getRealTime() : int
		{
			return getTimer();
		}
		
		public function logToConsole( message : String ) : void
		{
			_q3console.send( "q3console", "log", message );
		}
		
		/**********************************************************************************************************/
		/** EVENTS                                                                                               **/
		/**********************************************************************************************************/
		
		private function onFrame( e : Event ):void
		{
			_renderManager.beginFrame();
			_inputManager.update();
			
			_swc.swcFrame( getTimer() );
			
			if ( !_soundChannel )
			{
				_lastSampleDataPosition = 0;
				
				_soundChannel = _sound.play();
				_soundChannel.addEventListener( Event.SOUND_COMPLETE, soundCompleteHandler );
			}
			
			_renderManager.endFrame();
			_renderManager.render_time = 0;
			
			_last_time = getTimer();
		}
		
		private function lcStatusHandler( event : StatusEvent ) : void
		{
		}
		
		private function soundCompleteHandler( e : Event ) : void
		{
			_soundChannel.removeEventListener( Event.SOUND_COMPLETE, soundCompleteHandler );
			_soundChannel = null;
		}
		
		private function sampleDataHandler( event : SampleDataEvent ) : void
		{
			var delta : int = event.position - _lastSampleDataPosition;
			
			if ( delta > 4096 ) delta = 4096;
			
			_swc.swcWriteSoundData( event.data, delta );
			
			_lastSampleDataPosition = event.position;
		}
	}
}