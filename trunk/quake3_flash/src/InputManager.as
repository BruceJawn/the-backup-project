package  
{
	import flash.ui.Mouse;
	import flash.display.Stage;
	import flash.events.MouseEvent;
	import flash.events.KeyboardEvent;
	import flash.events.TimerEvent;
	import flash.utils.Timer;
	
	/**
	 * @author Pierre-Yves Gatouillat ==>> gatouillatpy@gmail.com
	 */
	public class InputManager
	{
		/**********************************************************************************************************/
		/** CONSTANTS                                                                                            **/
		/**********************************************************************************************************/
		
		private const MOUSE_SPEED : Number = 1.0;
		
		private const MOUSE_BUTTON_LEFT : int = 0x00;
		private const MOUSE_BUTTON_RIGHT : int = 0x01;
		
		/**********************************************************************************************************/
		/** MEMBERS                                                                                              **/
		/**********************************************************************************************************/
		
		private var engine : Quake3;
		
		private var swc : Object;
		
		private var stage : Stage;
		
		private var mouse_lock : Boolean;
		private var mouse_lock_ex : Boolean;
		
		private var old_mouseX : Number;
		private var old_mouseY : Number;
		
		private var key_states : Array;
		
		/**********************************************************************************************************/
		/** CONSTRUCTOR                                                                                          **/
		/**********************************************************************************************************/

		public function InputManager( _engine : Quake3 ) 
		{
			engine = _engine;
			
			swc = engine.swc;
			
			stage = engine.stage;
			stage.addEventListener( KeyboardEvent.KEY_DOWN, onKeyDown );
			stage.addEventListener( KeyboardEvent.KEY_UP, onKeyUp );
			stage.addEventListener( MouseEvent.MOUSE_DOWN, onMouseDown );
			stage.addEventListener( MouseEvent.MOUSE_UP, onMouseUp );
			
			mouse_lock = false;
			mouse_lock_ex = false;
			
			key_states = [];
		}
		
		/**********************************************************************************************************/
		/** METHODS                                                                                              **/
		/**********************************************************************************************************/
		
		public function update() : void
		{
			if ( mouse_lock || mouse_lock_ex )
			{
				var deltaX : int = Math.round( MOUSE_SPEED * (stage.mouseX - old_mouseX) );
				var deltaY : int = Math.round( MOUSE_SPEED * (stage.mouseY - old_mouseY) );
				
				swc.swcMouseMove( deltaX, deltaY );
				
				old_mouseX = stage.mouseX;
				old_mouseY = stage.mouseY;
			}
		}
		
		public function exit() : void
		{
			stage.removeEventListener( KeyboardEvent.KEY_DOWN, onKeyDown );
			stage.removeEventListener( KeyboardEvent.KEY_UP, onKeyUp );
			stage.removeEventListener( MouseEvent.MOUSE_DOWN, onMouseDown );
			stage.removeEventListener( MouseEvent.MOUSE_UP, onMouseUp );
		}
		
		private function toggleMouseLock() : void
		{
			// hopefully one day we'll have a real mouse capture...
			
			if ( mouse_lock )
			{
				mouse_lock = false;
				
				Mouse.show();
			}
			else
			{
				mouse_lock = true;
				mouse_lock_ex = false;
				
				old_mouseX = stage.mouseX;
				old_mouseY = stage.mouseY;
				
				Mouse.hide();
			}
		}
		
		private function toggleMouseLockEx() : void
		{
			if ( mouse_lock_ex )
			{
				mouse_lock_ex = false;
				
				Mouse.show();
			}
			else
			{
				mouse_lock_ex = true;
				mouse_lock = false;
				
				old_mouseX = stage.mouseX;
				old_mouseY = stage.mouseY;
				
				Mouse.hide();
			}
		}

		/**********************************************************************************************************/
		/** EVENTS                                                                                               **/
		/**********************************************************************************************************/

		private function onKeyDown( e : KeyboardEvent ) : void
		{
			if ( e.keyCode == 123 ) toggleMouseLock();
			
			if ( key_states[e.keyCode] ) return;
			
			swc.swcKeyDown( e.keyCode );
			
			key_states[e.keyCode] = true;
			
			//trace( "onKeyDown " + e.keyCode );
		}
		
		private function onKeyUp( e : KeyboardEvent ) : void
		{
			swc.swcKeyUp( e.keyCode );
			
			key_states[e.keyCode] = false;
			
			//trace( "onKeyUp " + e.keyCode );
		}
		
		private function onMouseDown( e : MouseEvent ) : void 
		{
			if ( mouse_lock )
				swc.swcMouseDown( MOUSE_BUTTON_LEFT );
			else
				toggleMouseLockEx();
			
			//trace( "onMouseDown" );
		}
		
		private function onMouseUp( e : MouseEvent ) : void 
		{
			if ( mouse_lock )
				swc.swcMouseUp( MOUSE_BUTTON_LEFT );
			else
				toggleMouseLockEx();
			
			//trace( "onMouseUp" );
		}
	}
}