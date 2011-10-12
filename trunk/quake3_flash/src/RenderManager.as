package  
{
	import com.adobe.utils.AGALMiniAssembler;
	
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.Stage3D;
	import flash.display.StageAlign;
	import flash.display.StageScaleMode;
	import flash.display3D.Context3D;
	import flash.display3D.Context3DBlendFactor;
	import flash.display3D.Context3DClearMask;
	import flash.display3D.Context3DCompareMode;
	import flash.display3D.Context3DProgramType;
	import flash.display3D.Context3DTextureFormat;
	import flash.display3D.Context3DTriangleFace;
	import flash.display3D.Context3DVertexBufferFormat;
	import flash.display3D.IndexBuffer3D;
	import flash.display3D.Program3D;
	import flash.display3D.VertexBuffer3D;
	import flash.display3D.textures.Texture;
	import flash.events.Event;
	import flash.events.MouseEvent;
	import flash.events.TimerEvent;
	import flash.geom.Matrix3D;
	import flash.geom.Rectangle;
	import flash.geom.Vector3D;
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	import flash.utils.Timer;
	import flash.utils.getTimer;
	
	import org.osflash.signals.Signal;

	/**
	 * @author Pierre-Yves Gatouillat ==>> gatouillatpy@gmail.com
	 */
	public class RenderManager
	{
		/**********************************************************************************************************/
		/** CONSTANTS                                                                                            **/
		/**********************************************************************************************************/
		
		private const MOLEHILL_BEGIN : uint						= 0x00010000;
		private const MOLEHILL_END : uint						= 0x00020000;
		
		private const MOLEHILL_CLEAR : uint						= 0x00110000;
		private const MOLEHILL_CLEAR_EX : uint					= 0x00120000;
		private const MOLEHILL_FLUSH : uint						= 0x00130000;
		
		private const MOLEHILL_BIND : uint						= 0x00210000;
		
		private const MOLEHILL_SET_DEPTHWRITE : uint			= 0x00310000;
		private const MOLEHILL_SET_DEPTHFUNC : uint				= 0x00320000;
		private const MOLEHILL_SET_BLENDING : uint				= 0x00330000;
		private const MOLEHILL_SET_VIEWPORT : uint				= 0x00340000;
		private const MOLEHILL_SET_CULLING : uint				= 0x00350000;
		private const MOLEHILL_SET_ALPHATEST : uint				= 0x00360000;
		private const MOLEHILL_SET_ALPHAFUNC : uint				= 0x00370000;
		private const MOLEHILL_SET_POLYGONOFFSET : uint			= 0x00380000;
		private const MOLEHILL_SET_DEPTHRANGE : uint			= 0x00390000;
		private const MOLEHILL_SET_TEXTUREWRAPMODE : uint		= 0x00400000;
		
		private const MOLEHILL_MATRIX : uint					= 0x00410000;
		private const MOLEHILL_VERTICES : uint					= 0x00420000;
		private const MOLEHILL_INDICES : uint					= 0x00430000;
		private const MOLEHILL_VERTEX_POINTER : uint			= 0x00440000;
		private const MOLEHILL_COLOR_POINTER : uint				= 0x00450000;
		private const MOLEHILL_TEXCOORD_POINTER : uint			= 0x00460000;
		private const MOLEHILL_INDEX_POINTER : uint				= 0x00470000;
		
		private const MOLEHILL_DRAW : uint						= 0x00510000;
		private const MOLEHILL_DRAW_FAST : uint					= 0x00520000;
		
		private const MOLEHILL_UPLOAD_CACHE : uint				= 0x00610000;
		private const MOLEHILL_DRAW_CACHE : uint				= 0x00620000;
		
		private const MOLEHILL_TRIANGLE_LIST : uint				= 0x00000001;
		private const MOLEHILL_TRIANGLE_STRIP : uint			= 0x00000002;
		private const MOLEHILL_TRIANGLE_FAN : uint				= 0x00000003;
		private const MOLEHILL_QUAD_LIST : uint					= 0x00000011;
		private const MOLEHILL_POLYGON : uint					= 0x00000024;
		
		private const MOLEHILL_FALSE : uint						= 0x00000000;
		private const MOLEHILL_TRUE : uint						= 0x00000001;
		
		private const MOLEHILL_ALWAYS : uint					= 0x00000001;
		private const MOLEHILL_LESS_OR_EQUAL : uint				= 0x00000002;
		private const MOLEHILL_EQUAL : uint						= 0x00000003;
		private const MOLEHILL_GREATER_OR_EQUAL : uint			= 0x00000004;
		
		private const MOLEHILL_ONE : uint						= 0x00000001;
		private const MOLEHILL_ZERO : uint						= 0x00000002;
		private const MOLEHILL_SRC_ALPHA : uint					= 0x00000011;
		private const MOLEHILL_SRC_COLOR : uint					= 0x00000012;
		private const MOLEHILL_ONE_MINUS_SRC_ALPHA : uint		= 0x00000021;
		private const MOLEHILL_ONE_MINUS_SRC_COLOR : uint		= 0x00000022;
		private const MOLEHILL_DST_ALPHA : uint					= 0x00000031;
		private const MOLEHILL_DST_COLOR : uint					= 0x00000032;
		private const MOLEHILL_ONE_MINUS_DST_ALPHA : uint		= 0x00000041;
		private const MOLEHILL_ONE_MINUS_DST_COLOR : uint		= 0x00000042;
		
		private const MOLEHILL_NONE : uint						= 0x00000001;
		private const MOLEHILL_FRONT : uint						= 0x00000002;
		private const MOLEHILL_BACK : uint						= 0x00000003;
		
		private const MOLEHILL_CLAMP : uint						= 0x00000001;
		private const MOLEHILL_REPEAT : uint					= 0x00000002;

		/**********************************************************************************************************/
		/** MEMBERS                                                                                              **/
		/**********************************************************************************************************/
		
		private var engine : Quake3;
		
		private var sp0aa : Program3D;
		private var sp0ba : Program3D;
		private var sp0ca : Program3D;
		private var sp0ab : Program3D;
		private var sp0bb : Program3D;
		private var sp0cb : Program3D;
		private var sp1a : Program3D;
		private var sp1b : Program3D;
		private var sp1c : Program3D;
		private var ib : IndexBuffer3D; 
		private var vb0 : VertexBuffer3D; 
		private var vb1 : VertexBuffer3D;
		
		private var vertices_fvb : VertexBuffer3D; 
		private var colors_fvb : VertexBuffer3D; 
		private var texcoords_fvb : VertexBuffer3D; 
		private var indices_fib : IndexBuffer3D; 
		
		private var vertex_count : int;
		private var index_count : int;
		
		private var vertex_viewport_offset : int;
		private var index_viewport_offset : int;
		
		private var caches : Array;
		
		private var matrix : Matrix3D;
		
		private var viewport_center : Vector.<Number>;
		private var viewport_factor : Vector.<Number>;
		private var viewport_offset : Vector.<Number>;
		
		private var depthrange_value : Vector.<Number>;
		
		private var ram : ByteArray;
		
		private var textures : Array;
		private var tid : int;
		
		private var matrix_values : Vector.<Number>;
		
		private var vertices : Vector.<Number>;
		private var indices : Vector.<uint>;
		
		private var use_texturing : Boolean;
		
		private var texture_count : int;
		
		private var old_time : int;
		
		private var depth_write : Boolean;
		private var depth_func : String;
		
		private var blend_src_op : String;
		private var blend_dst_op : String;
		
		private var alpha_test : Boolean;
		private var alpha_func : String;
		private var alpha_ref : Number;
		
		private var use_viewport_hack : Boolean;
		private var use_depthrange_hack : Boolean;
		
		private var stage3D : Stage3D;
		private var context3D : Context3D;
		
		public var render_time : uint;
		
		public var onInitComplete : Signal;
		
		/**********************************************************************************************************/
		/** CONSTRUCTOR                                                                                          **/
		/**********************************************************************************************************/
		
		public function RenderManager( _engine : Quake3 ) 
		{
			engine = _engine;
			
			engine.stage.scaleMode = StageScaleMode.NO_SCALE;
			engine.stage.align = StageAlign.TOP_LEFT;
			
			engine.stage.stage3Ds[0].addEventListener( Event.CONTEXT3D_CREATE, stageNotificationHandler );
			engine.stage.stage3Ds[0].requestContext3D();
			
			onInitComplete = new Signal;
			
			ram = engine.ram;
		}
		
		/**********************************************************************************************************/
		/** METHODS                                                                                              **/
		/**********************************************************************************************************/
		
		public function beginFrame() : void
		{
			context3D.clear( 0.0, 0.0, 0.0, 1.0, 1.0 );
		}
		
		public function endFrame() : void
		{
			context3D.present();
		}
		
		private function stageNotificationHandler( event : Event ) : void
		{
			stage3D = event.target as Stage3D;
			context3D = stage3D.context3D;
			
			context3D.enableErrorChecking = false; // true in debug mode
			
			init();
			
			onInitComplete.dispatch();
		}
		
		private function getTimeStamp() : String
		{
			var new_time : int = getTimer();
			
			var delta : int = new_time - old_time;
			
			old_time = new_time;
			
			return "[" + new_time + " | " + delta + " ms]";
		}
		
		public function init() : void 
		{
			getTimeStamp();
			
			context3D.configureBackBuffer( 800, 600, 0, true );
			
			render_time = 0;
			
			texture_count = 0;
			
			use_viewport_hack = false;
			use_depthrange_hack = false;
			
			matrix_values = new Vector.<Number>();
			
			vertices = new Vector.<Number>();
			indices = new Vector.<uint>();
			
			vertex_count = 0;
			index_count = 0;
			
			vertex_viewport_offset = 0;
			index_viewport_offset = 0;
			
			caches = new Array();
			
			depth_write = true;
			depth_func = Context3DCompareMode.LESS_EQUAL;
			
			alpha_test = false;
			alpha_func = Context3DCompareMode.GREATER_EQUAL;
			
			textures = new Array();
			tid = 0;
			
			viewport_center = Vector.<Number>( [1.0, 1.0, 0.0, 0.0] );
			viewport_factor = Vector.<Number>( [1.0, 1.0, 1.0, 1.0] );
			viewport_offset = Vector.<Number>( [0.0, 0.0, 0.0, 0.0] );
			
			depthrange_value = Vector.<Number>( [0.0, 1.0, 0.0, 0.0] );
			
			var vs0a : AGALMiniAssembler = new AGALMiniAssembler;
			vs0a.assemble
			(
				Context3DProgramType.VERTEX,
				"dp4 vt0.x, va0, vc0		\n" +
				"dp4 vt0.y, va0, vc1		\n" +
				"dp4 vt0.z, va0, vc2		\n" +
				"dp4 vt0.w, va0, vc3		\n" +
				"mov op, vt0.xyzw			\n" +
				"mov v0, va1.xyzw       	\n" +
				"mov v1, va2.xyzw       	\n"
			);
			
			var vs0b : AGALMiniAssembler = new AGALMiniAssembler;
			vs0b.assemble
			(
				Context3DProgramType.VERTEX,
				"dp4 vt0.x, va0, vc0		\n" +
				"dp4 vt0.y, va0, vc1		\n" +
				"dp4 vt0.z, va0, vc2		\n" +
				"dp4 vt0.w, va0, vc3		\n" +
				////////////////////////////////////
				"rcp vt1.w, vt0.w			\n" + //
				"mul vt1.x, vt0.x, vt1.w	\n" + //
				"mul vt1.y, vt0.y, vt1.w	\n" + //
				"mul vt1.z, vt0.z, vt1.w	\n" + //
				"add vt2, vt1, vc4			\n" + // ugly hack to simulate the effect of glViewport
				"mul vt3, vt2, vc5			\n" + //
				"mov vt3.w, vc5.w			\n" + //
				"sub vt4, vt3, vc4			\n" + //
				"add op, vt4, vc6			\n" + //
				////////////////////////////////////
				"mov v0, va1.xyzw       	\n" +
				"mov v1, va2.xyzw       	\n"
			);
			
			var vs0c : AGALMiniAssembler = new AGALMiniAssembler();
			vs0c.assemble
			(
				AGALMiniAssembler.VERTEX,
				"dp4 vt0.x, va0, vc0		\n" +
				"dp4 vt0.y, va0, vc1		\n" +
				"dp4 vt0.z, va0, vc2		\n" +
				"dp4 vt0.w, va0, vc3		\n" +
				////////////////////////////////////
				"add op, vt0, vc7			\n" + // ugly hack to simulate the effect of glDepthRange
				////////////////////////////////////
				"mov v0, va1.xyzw       	\n" +
				"mov v1, va2.xyzw       	\n"
			);
			
			var ps0a : AGALMiniAssembler = new AGALMiniAssembler(); 
			ps0a.assemble
			(
				AGALMiniAssembler.FRAGMENT,		
				"tex ft1, v1, fs1 <2d,linear,clamp>		\n" +
				"mul ft0, ft1, v0						\n" +
				"mov oc, ft0							\n"
			);
			
			var ps0b : AGALMiniAssembler = new AGALMiniAssembler(); 
			ps0b.assemble
			(
				AGALMiniAssembler.FRAGMENT,		
				"tex ft1, v1, fs1 <2d,linear,repeat>	\n" +
				"mul ft0, ft1, v0						\n" +
				"mov oc, ft0							\n"
			);
			
			sp0aa = context3D.createProgram();
			sp0aa.upload( vs0a.agalcode, ps0a.agalcode );
			
			sp0ba = context3D.createProgram();
			sp0ba.upload( vs0b.agalcode, ps0a.agalcode );
			
			sp0ca = context3D.createProgram();
			sp0ca.upload( vs0c.agalcode, ps0a.agalcode );
			
			sp0ab = context3D.createProgram();
			sp0ab.upload( vs0a.agalcode, ps0b.agalcode );
			
			sp0bb = context3D.createProgram();
			sp0bb.upload( vs0b.agalcode, ps0b.agalcode );
			
			sp0cb = context3D.createProgram();
			sp0cb.upload( vs0c.agalcode, ps0b.agalcode );
			
			var vs1a : AGALMiniAssembler = new AGALMiniAssembler();
			vs1a.assemble
			(
				AGALMiniAssembler.VERTEX,
				"dp4 vt0.x, va0, vc0		\n" +
				"dp4 vt0.y, va0, vc1		\n" +
				"dp4 vt0.z, va0, vc2		\n" +
				"dp4 vt0.w, va0, vc3		\n" +
				"mov op, vt0.xyzw			\n" +
				"mov v0, va1.xyzw			\n"
			);
			
			var vs1b : AGALMiniAssembler = new AGALMiniAssembler();
			vs1b.assemble
			(
				AGALMiniAssembler.VERTEX,
				"dp4 vt0.x, va0, vc0		\n" +
				"dp4 vt0.y, va0, vc1		\n" +
				"dp4 vt0.z, va0, vc2		\n" +
				"dp4 vt0.w, va0, vc3		\n" +
				////////////////////////////////////
				"rcp vt1.w, vt0.w			\n" + //
				"mul vt1.x, vt0.x, vt1.w	\n" + //
				"mul vt1.y, vt0.y, vt1.w	\n" + //
				"mul vt1.z, vt0.z, vt1.w	\n" + //
				"add vt2, vt1, vc4			\n" + // ugly hack to simulate the effect of glViewport
				"mul vt3, vt2, vc5			\n" + //
				"mov vt3.w, vc5.w			\n" + //
				"sub vt4, vt3, vc4			\n" + //
				"add op, vt4, vc6			\n" + //
				////////////////////////////////////
				"mov v0, va1.xyzw			\n"
			);
			
			var ps1 : AGALMiniAssembler = new AGALMiniAssembler(); 
			ps1.assemble
			(
				AGALMiniAssembler.FRAGMENT,		
				"mov oc, v0							\n"
			);
			
			sp1a = context3D.createProgram();
			sp1a.upload( vs1a.agalcode, ps1.agalcode );
			
			sp1b = context3D.createProgram();
			sp1b.upload( vs1b.agalcode, ps1.agalcode );
			
			var null_indices : Vector.<uint> = new Vector.<uint>( 4096, true );
			var null_vertices0 : Vector.<Number> = new Vector.<Number>( 16384 * 10, true );
			var null_vertices1 : Vector.<Number> = new Vector.<Number>( 16384 * 8, true );
			
			ib = context3D.createIndexBuffer( 4096 );
			ib.uploadFromVector( null_indices, 0, 4096 );
			
			vb0 = context3D.createVertexBuffer( 16384, 10 ); // vertex + color + texcoord
			vb0.uploadFromVector( null_vertices0, 0, 16384 );
			
			vb1 = context3D.createVertexBuffer( 16384, 8 ); // vertex + color
			vb1.uploadFromVector( null_vertices1, 0, 16384 );
			
			var null_indices_fib : Vector.<uint> = new Vector.<uint>( 4096, true );
			var null_vertices_fvb : Vector.<Number> = new Vector.<Number>( 16384 * 4, true );
			var null_colors_fvb : Vector.<Number> = new Vector.<Number>( 16384 * 1, true );
			var null_texcoords_fvb : Vector.<Number> = new Vector.<Number>( 16384 * 2, true );
			
			vertices_fvb = context3D.createVertexBuffer( 16384, 4 );
			vertices_fvb.uploadFromVector( null_vertices_fvb, 0, 16384 );
			
			colors_fvb = context3D.createVertexBuffer( 16384, 1 );
			colors_fvb.uploadFromVector( null_colors_fvb, 0, 16384 );
			
			texcoords_fvb = context3D.createVertexBuffer( 16384, 2 );
			texcoords_fvb.uploadFromVector( null_texcoords_fvb, 0, 16384 );
			
			indices_fib = context3D.createIndexBuffer( 4096 );
			indices_fib.uploadFromVector( null_indices_fib, 0, 4096 );
		}
		
		public function exit() : void 
		{
			context3D.clear();
			context3D.present();
			context3D.dispose();
		}
		
		public function execute( data_ptr : uint ) : void
		{
			//trace( getTimeStamp() + " begin execute( " + data_ptr + " )" );
			
			var time : uint = getTimer();
			
			ram.position = data_ptr;
			
			var last_opcode : uint;
			var opcode : uint;
			var count : uint = 0;
			
			if ( ( opcode = ram.readUnsignedInt() ) == MOLEHILL_BEGIN )
			{
				while ( ( opcode = ram.readUnsignedInt() ) != MOLEHILL_END )
				{
					count++;
					
					var red : Number;
					var green : Number;
					var blue : Number;
					var alpha : Number;
					var depth : Number;
					var stencil : int;
					var id : int;
					var ptr : uint;
					var len : int;
					var factor : Number;
					var units : Number;
					var near : Number;
					var far : Number;
					
					//trace( "masm opcode : " + opcode + " last_opcode : " + last_opcode );
					
					if ( opcode == MOLEHILL_CLEAR )
					{
						red = ram.readFloat();
						green = ram.readFloat();
						blue = ram.readFloat();
						alpha = ram.readFloat();
						depth = ram.readFloat();
						stencil = ram.readInt();
						
						context3D.clear( red, green, blue, alpha, depth, stencil, Context3DClearMask.COLOR );
					}
					if ( opcode == MOLEHILL_CLEAR_EX )
					{
						depth = ram.readFloat();
						stencil = ram.readInt();
						
						context3D.clear( red, green, blue, alpha, depth, stencil, Context3DClearMask.DEPTH | Context3DClearMask.STENCIL );
					}
					else if ( opcode == MOLEHILL_FLUSH )
					{
						// do nothing
					}
					else if ( opcode == MOLEHILL_MATRIX )
					{
						for ( var k : int = 0 ; k < 16 ; k++ )
							matrix_values[k] = ram.readFloat();
						
						matrix = new Matrix3D( matrix_values );
						matrix.transpose();
					}
					else if ( opcode == MOLEHILL_VERTICES + 0x0A )
					{
						use_texturing = true;
						
						vertex_count += ram.readInt();
						
						for ( var i : int = vertex_viewport_offset * 10 ; i < vertex_count * 10 ; i++ )
							vertices[i] = ram.readFloat();
						
						vertex_viewport_offset = vertex_count;
					}
					else if ( opcode == MOLEHILL_VERTICES + 0x08 )
					{
						use_texturing = false;

						vertex_count += ram.readInt();
						
						for ( var j : int = vertex_viewport_offset * 8 ; j < vertex_count * 8 ; j++ )
							vertices[j] = ram.readFloat();
						
						vertex_viewport_offset = vertex_count;
					}
					else if ( opcode == MOLEHILL_INDICES )
					{
						index_count += ram.readInt();

						for ( var n : int = index_viewport_offset ; n < index_count ; n++ )
							indices[n] = ram.readInt();
						
						index_viewport_offset = index_count;
					}
					else if ( opcode == MOLEHILL_VERTEX_POINTER )
					{
						ptr = ram.readUnsignedInt();
						len = ram.readInt();
						
						vertices_fvb.uploadFromByteArray( ram, ptr, 0, len );
						
						vertex_viewport_offset = 0;
						vertex_count = len;
					}
					else if ( opcode == MOLEHILL_COLOR_POINTER )
					{
						ptr = ram.readUnsignedInt();
						len = ram.readInt();
						
						colors_fvb.uploadFromByteArray( ram, ptr, 0, len );
					}
					else if ( opcode == MOLEHILL_TEXCOORD_POINTER )
					{
						use_texturing = true;
						
						ptr = ram.readUnsignedInt();
						len = ram.readInt();
						
						texcoords_fvb.uploadFromByteArray( ram, ptr, 0, len );
					}
					else if ( opcode == MOLEHILL_INDEX_POINTER )
					{
						ptr = ram.readUnsignedInt();
						len = ram.readInt();
						
						indices_fib.uploadFromByteArray( ram, ptr, 0, len );

						index_viewport_offset = 0;
						index_count = len;
					}
					else if ( opcode == MOLEHILL_UPLOAD_CACHE )
					{
						id = ram.readInt();
						
						uploadCache( id );
					}
					else if ( opcode == MOLEHILL_DRAW_CACHE )
					{
						id = ram.readInt();
						
						drawCache( id );
					}
					else if ( opcode == MOLEHILL_DRAW )
					{
						draw();
					}
					else if ( opcode == MOLEHILL_DRAW_FAST )
					{
						drawFast();
					}
					else if ( opcode == MOLEHILL_BIND )
					{
						id = ram.readInt();
						
						bindTexture( id );
					}
					else if ( opcode == MOLEHILL_SET_BLENDING + (MOLEHILL_ONE << 8) + (MOLEHILL_ZERO << 0) )
					{
						blend_src_op = Context3DBlendFactor.ONE;
						blend_dst_op = Context3DBlendFactor.ZERO;
						
						context3D.setBlendFactors( blend_src_op, blend_dst_op );
					}
					else if ( opcode == MOLEHILL_SET_BLENDING + (MOLEHILL_ONE << 8) + (MOLEHILL_ONE << 0) )
					{
						blend_src_op = Context3DBlendFactor.ONE;
						blend_dst_op = Context3DBlendFactor.ONE;
						
						context3D.setBlendFactors( blend_src_op, blend_dst_op );
					}
					else if ( opcode == MOLEHILL_SET_BLENDING + (MOLEHILL_SRC_ALPHA << 8) +( MOLEHILL_ONE_MINUS_SRC_ALPHA << 0) )
					{
						blend_src_op = Context3DBlendFactor.SOURCE_ALPHA;
						blend_dst_op = Context3DBlendFactor.ONE_MINUS_SOURCE_ALPHA;
						
						context3D.setBlendFactors( blend_src_op, blend_dst_op );
					}
					else if ( opcode == MOLEHILL_SET_BLENDING + (MOLEHILL_ZERO << 8) + (MOLEHILL_ONE_MINUS_SRC_COLOR << 0) )
					{
						blend_src_op = Context3DBlendFactor.ZERO;
						blend_dst_op = Context3DBlendFactor.ONE_MINUS_SOURCE_COLOR;
						
						context3D.setBlendFactors( blend_src_op, blend_dst_op );
					}
					else if ( opcode == MOLEHILL_SET_BLENDING + (MOLEHILL_DST_COLOR << 8) + (MOLEHILL_ZERO << 0) )
					{
						blend_src_op = Context3DBlendFactor.DESTINATION_COLOR;
						blend_dst_op = Context3DBlendFactor.ZERO;
						
						context3D.setBlendFactors( blend_src_op, blend_dst_op );
					}
					else if ( opcode == MOLEHILL_SET_BLENDING + (MOLEHILL_DST_COLOR << 8) + (MOLEHILL_SRC_COLOR << 0) )
					{
						blend_src_op = Context3DBlendFactor.DESTINATION_COLOR;
						blend_dst_op = Context3DBlendFactor.SOURCE_COLOR;
						
						context3D.setBlendFactors( blend_src_op, blend_dst_op );
					}
					else if ( opcode == MOLEHILL_SET_BLENDING + (MOLEHILL_DST_COLOR << 8) + (MOLEHILL_ONE_MINUS_DST_ALPHA << 0) )
					{
						blend_src_op = Context3DBlendFactor.DESTINATION_COLOR;
						blend_dst_op = Context3DBlendFactor.ONE_MINUS_DESTINATION_ALPHA;
						
						context3D.setBlendFactors( blend_src_op, blend_dst_op );
					}
					else if ( opcode == MOLEHILL_SET_BLENDING + (MOLEHILL_DST_COLOR << 8) + (MOLEHILL_ONE << 0) )
					{
						blend_src_op = Context3DBlendFactor.DESTINATION_COLOR;
						blend_dst_op = Context3DBlendFactor.ONE;
						
						context3D.setBlendFactors( blend_src_op, blend_dst_op );
					}
					else if ( opcode == MOLEHILL_SET_BLENDING + (MOLEHILL_DST_COLOR << 8) + (MOLEHILL_SRC_ALPHA << 0) )
					{
						blend_src_op = Context3DBlendFactor.DESTINATION_COLOR;
						blend_dst_op = Context3DBlendFactor.SOURCE_ALPHA;
						
						context3D.setBlendFactors( blend_src_op, blend_dst_op );
					}
					else if ( opcode == MOLEHILL_SET_TEXTUREWRAPMODE + MOLEHILL_CLAMP )
					{
						if ( textures[tid] )
							textures[tid].wrap_mode = MOLEHILL_CLAMP;
					}
					else if ( opcode == MOLEHILL_SET_TEXTUREWRAPMODE + MOLEHILL_REPEAT )
					{
						if ( textures[tid] )
							textures[tid].wrap_mode = MOLEHILL_REPEAT;
					}
					else if ( opcode == MOLEHILL_SET_DEPTHWRITE + MOLEHILL_FALSE )
					{
						depth_write = false;
						
						context3D.setDepthTest( depth_write, depth_func );
					}
					else if ( opcode == MOLEHILL_SET_DEPTHWRITE + MOLEHILL_TRUE )
					{
						depth_write = true;
						
						context3D.setDepthTest( depth_write, depth_func );
					}
					else if ( opcode == MOLEHILL_SET_ALPHATEST + MOLEHILL_FALSE )
					{
						alpha_test = false;
					}
					else if ( opcode == MOLEHILL_SET_ALPHATEST + MOLEHILL_TRUE )
					{
						alpha_test = true;
					}
					else if ( opcode == MOLEHILL_SET_DEPTHFUNC + MOLEHILL_ALWAYS )
					{
						depth_func = Context3DCompareMode.ALWAYS;
						
						context3D.setDepthTest( depth_write, depth_func );
					}
					else if ( opcode == MOLEHILL_SET_DEPTHFUNC + MOLEHILL_EQUAL )
					{
						depth_func = Context3DCompareMode.EQUAL;
						
						context3D.setDepthTest( depth_write, depth_func );
					}
					else if ( opcode == MOLEHILL_SET_DEPTHFUNC + MOLEHILL_LESS_OR_EQUAL )
					{
						depth_func = Context3DCompareMode.LESS_EQUAL;
						
						context3D.setDepthTest( depth_write, depth_func );
					}
					else if ( opcode == MOLEHILL_SET_ALPHAFUNC + MOLEHILL_GREATER_OR_EQUAL )
					{
						alpha_func = Context3DCompareMode.GREATER_EQUAL;
						alpha_ref = ram.readFloat();
					}
					else if ( opcode == MOLEHILL_SET_VIEWPORT )
					{
						var vx : int = ram.readInt();
						var vy : int = ram.readInt();
						var vw : int = ram.readInt();
						var vh : int = ram.readInt();
						
						setViewport( vx, vy, vw, vh );
					}
					else if ( opcode == MOLEHILL_SET_POLYGONOFFSET + MOLEHILL_FALSE )
					{
						factor = ram.readFloat();
						units = ram.readFloat();
						
						setPolygonOffset( false, factor, units );
					}
					else if ( opcode == MOLEHILL_SET_POLYGONOFFSET + MOLEHILL_TRUE )
					{
						factor = ram.readFloat();
						units = ram.readFloat();
						
						setPolygonOffset( true, factor, units );
					}
					else if ( opcode == MOLEHILL_SET_DEPTHRANGE + MOLEHILL_FALSE )
					{
						near = ram.readFloat();
						far = ram.readFloat();
						
						setDepthRange( false, near, far );
					}
					else if ( opcode == MOLEHILL_SET_DEPTHRANGE + MOLEHILL_TRUE )
					{
						near = ram.readFloat();
						far = ram.readFloat();
						
						setDepthRange( true, near, far );
					}
					else if ( opcode == MOLEHILL_SET_CULLING + MOLEHILL_NONE )
					{
						context3D.setCulling( Context3DTriangleFace.NONE );
					}
					else if ( opcode == MOLEHILL_SET_CULLING + MOLEHILL_FRONT )
					{
						context3D.setCulling( Context3DTriangleFace.FRONT );
					}
					else if ( opcode == MOLEHILL_SET_CULLING + MOLEHILL_BACK )
					{
						context3D.setCulling( Context3DTriangleFace.BACK );
					}
					else
					{
						trace( "invalid masm opcode : " + opcode + " last_opcode : " + last_opcode );
					}
					
					last_opcode = opcode;
				}
			}
			
			//trace( getTimeStamp() + " end execute( " + data_ptr + " ) " + count + " instructions, " + (ram.position - data_ptr) + " bytes read." );
			
			render_time += getTimer() - time;
		}
		
		private function uploadCache( id : int ) : void
		{
			//trace( getTimeStamp() + " uploadCache( " + id + " )" );
			
			var index_buffer : IndexBuffer3D;
			var vertex_buffer : VertexBuffer3D;
			
			if ( index_count > 0 )
			{
				index_buffer = context3D.createIndexBuffer( index_count );
				index_buffer.uploadFromVector( indices, 0, index_count );
				
				if ( use_texturing )
				{
					vertex_buffer = context3D.createVertexBuffer( vertex_count, 10 ); // vertex + color + texcoord
					vertex_buffer.uploadFromVector( vertices, 0, vertex_count );
				}
				else
				{
					vertex_buffer = context3D.createVertexBuffer( vertex_count, 8 ); // vertex + color
					vertex_buffer.uploadFromVector( vertices, 0, vertex_count );
				}

				var cache : Object =
				{
					vertex_count: vertex_count,
					index_count: index_count,
					
					vertex_buffer: vertex_buffer,
					index_buffer: index_buffer,
					
					use_texturing: use_texturing
				};
				
				caches[id] = cache;
			}
		}
		
		private function drawCache( id : int ) : void
		{
			//trace( getTimeStamp() + " drawCache( " + id + " )" );
			
			var index_buffer : IndexBuffer3D;
			var vertex_buffer : VertexBuffer3D;
			
			var cache : Object = caches[id];
			
			if ( cache )
			{
				use_texturing = cache.use_texturing;
				
				vertex_count = cache.vertex_count;
				index_count = cache.index_count;
				
				vertex_buffer = cache.vertex_buffer;
				index_buffer = cache.index_buffer;
				
				if ( use_texturing )
				{
					if ( textures[tid] )
					{
						if ( textures[tid].data )
						{
							context3D.setTextureAt( 1, textures[tid].data );
							
							if ( textures[tid].wrap_mode == MOLEHILL_CLAMP )
							{
								if ( use_depthrange_hack )
									context3D.setProgram( sp0ca );
								else if ( use_viewport_hack )
									context3D.setProgram( sp0ba );
								else
									context3D.setProgram( sp0aa );
							}
							else
							{
								if ( use_depthrange_hack )
									context3D.setProgram( sp0cb );
								else if ( use_viewport_hack )
									context3D.setProgram( sp0bb );
								else
									context3D.setProgram( sp0ab );
							}
							
							context3D.setVertexBufferAt( 0, vertex_buffer, 0, Context3DVertexBufferFormat.FLOAT_4 );
							context3D.setVertexBufferAt( 1, vertex_buffer, 4, Context3DVertexBufferFormat.FLOAT_4 );
							context3D.setVertexBufferAt( 2, vertex_buffer, 8, Context3DVertexBufferFormat.FLOAT_2 );
						}
						else
						{
							trace( "unloaded texture id=" + tid );
						}
					}
					else
					{
						trace( "invalid texture id=" + tid );
					}
				}
				else
				{
					if ( use_viewport_hack )
						context3D.setProgram( sp1b );
					else
						context3D.setProgram( sp1a );
					
					context3D.setVertexBufferAt( 0, vertex_buffer, 0, Context3DVertexBufferFormat.FLOAT_4 );
					context3D.setVertexBufferAt( 1, vertex_buffer, 4, Context3DVertexBufferFormat.FLOAT_4 );
				}
				
				context3D.setProgramConstantsFromMatrix( Context3DProgramType.VERTEX, 0, matrix, true );
				context3D.setProgramConstantsFromVector( Context3DProgramType.VERTEX, 4, viewport_center, 1 );
				context3D.setProgramConstantsFromVector( Context3DProgramType.VERTEX, 5, viewport_factor, 1 );
				context3D.setProgramConstantsFromVector( Context3DProgramType.VERTEX, 6, viewport_offset, 1 );
				context3D.setProgramConstantsFromVector( Context3DProgramType.VERTEX, 7, depthrange_value, 1 );
				
				//try
				//{
					context3D.drawTriangles( index_buffer, 0, index_count / 3 );
				//}
				//catch ( e : * )
				//{
				//	trace( e );
				//}
			}
			
			vertex_count = 0;
			index_count = 0;
			
			vertex_viewport_offset = 0;
			index_viewport_offset = 0;
		}
		
		private function draw() : void
		{
			//trace( getTimeStamp() + " draw()" );
			
			if ( index_count > 0 )
			{
				ib.uploadFromVector( indices, 0, index_count );
				
				if ( use_texturing )
				{
					vb0.uploadFromVector( vertices, 0, vertex_count );
					
					if ( textures[tid] )
					{
						if ( textures[tid].data )
						{
							context3D.setTextureAt( 1, textures[tid].data );
							
							if ( textures[tid].wrap_mode == MOLEHILL_CLAMP )
							{
								if ( use_depthrange_hack )
									context3D.setProgram( sp0ca );
								else if ( use_viewport_hack )
									context3D.setProgram( sp0ba );
								else
									context3D.setProgram( sp0aa );
							}
							else
							{
								if ( use_depthrange_hack )
									context3D.setProgram( sp0cb );
								else if ( use_viewport_hack )
									context3D.setProgram( sp0bb );
								else
									context3D.setProgram( sp0ab );
							}
							
							context3D.setVertexBufferAt( 0, vb0, 0, Context3DVertexBufferFormat.FLOAT_4 );
							context3D.setVertexBufferAt( 1, vb0, 4, Context3DVertexBufferFormat.FLOAT_4 );
							context3D.setVertexBufferAt( 2, vb0, 8, Context3DVertexBufferFormat.FLOAT_2 );
						}
						else
						{
							trace( "unloaded texture id=" + tid );
						}
					}
					else
					{
						trace( "invalid texture id=" + tid );
					}
				}
				else
				{
					vb1.uploadFromVector( vertices, 0, vertex_count );
					
					if ( use_viewport_hack )
						context3D.setProgram( sp1b );
					else
						context3D.setProgram( sp1a );
					
					context3D.setVertexBufferAt( 0, vb1, 0, Context3DVertexBufferFormat.FLOAT_4 );
					context3D.setVertexBufferAt( 1, vb1, 4, Context3DVertexBufferFormat.FLOAT_4 );
				}
				
				context3D.setProgramConstantsFromMatrix( Context3DProgramType.VERTEX, 0, matrix, true );
				context3D.setProgramConstantsFromVector( Context3DProgramType.VERTEX, 4, viewport_center, 1 );
				context3D.setProgramConstantsFromVector( Context3DProgramType.VERTEX, 5, viewport_factor, 1 );
				context3D.setProgramConstantsFromVector( Context3DProgramType.VERTEX, 6, viewport_offset, 1 );
				context3D.setProgramConstantsFromVector( Context3DProgramType.VERTEX, 7, depthrange_value, 1 );
				
				//try
				//{
					context3D.drawTriangles( ib, 0, index_count / 3 );
				//}
				//catch ( e : * )
				//{
				//	trace( e );
				//}
			}
			
			vertex_count = 0;
			index_count = 0;
			
			vertex_viewport_offset = 0;
			index_viewport_offset = 0;
		}
		
		private function drawFast() : void
		{
			//trace( getTimeStamp() + " drawFast()" );
			
			if ( index_count > 0 )
			{
				if ( use_texturing )
				{
					if ( textures[tid] )
					{
						if ( textures[tid].data )
						{
							context3D.setTextureAt( 1, textures[tid].data );
							
							if ( textures[tid].wrap_mode == MOLEHILL_CLAMP )
							{
								if ( use_depthrange_hack )
									context3D.setProgram( sp0ca );
								else if ( use_viewport_hack )
									context3D.setProgram( sp0ba );
								else
									context3D.setProgram( sp0aa );
							}
							else
							{
								if ( use_depthrange_hack )
									context3D.setProgram( sp0cb );
								else if ( use_viewport_hack )
									context3D.setProgram( sp0bb );
								else
									context3D.setProgram( sp0ab );
							}
							
							context3D.setVertexBufferAt( 0, vertices_fvb, 0, Context3DVertexBufferFormat.FLOAT_4 );
							context3D.setVertexBufferAt( 1, colors_fvb, 0, Context3DVertexBufferFormat.BYTES_4 );
							context3D.setVertexBufferAt( 2, texcoords_fvb, 0, Context3DVertexBufferFormat.FLOAT_2 );
						}
						else
						{
							trace( "unloaded texture id=" + tid );
						}
					}
					else
					{
						trace( "invalid texture id=" + tid );
					}
				}
				else
				{
					if ( use_viewport_hack )
						context3D.setProgram( sp1b );
					else
						context3D.setProgram( sp1a );
					
					context3D.setVertexBufferAt( 0, vertices_fvb, 0, Context3DVertexBufferFormat.FLOAT_4 );
					context3D.setVertexBufferAt( 1, colors_fvb, 0, Context3DVertexBufferFormat.BYTES_4 );
				}

				context3D.setProgramConstantsFromMatrix( Context3DProgramType.VERTEX, 0, matrix, true );
				context3D.setProgramConstantsFromVector( Context3DProgramType.VERTEX, 4, viewport_center, 1 );
				context3D.setProgramConstantsFromVector( Context3DProgramType.VERTEX, 5, viewport_factor, 1 );
				context3D.setProgramConstantsFromVector( Context3DProgramType.VERTEX, 6, viewport_offset, 1 );
				context3D.setProgramConstantsFromVector( Context3DProgramType.VERTEX, 7, depthrange_value, 1 );
				
				//try
				//{
					context3D.drawTriangles( indices_fib, 0, index_count / 3 );
				//}
				//catch ( e : * )
				//{
				//	trace( e );
				//}
			}
			
			vertex_count = 0;
			index_count = 0;

			vertex_viewport_offset = 0;
			index_viewport_offset = 0;
			
			use_texturing = false;
		}
		
		private function dropUnusedTexture() : void
		{
			//trace( getTimeStamp() + " dropUnusedTexture()" );
			
			var unused_texture : Object = null;
			
			for each ( var texture : Object in textures )
			{
				if ( texture.data != null )
				{
					if ( unused_texture == null || texture.time < unused_texture.time )
					{
						unused_texture = texture;
					}
				}
			}
			
			if ( unused_texture != null )
			{
				(unused_texture.data as Texture).dispose();
				
				unused_texture.data = null;
				
				texture_count--;
			}
		}
		
		private function reuploadTexture() : void
		{
			//trace( getTimeStamp() + " reuploadTexture( " + tid + " )" );
			
			if ( tid == 0 ) return;

			var texture : Object = textures[tid];
			var source : BitmapData = texture.sources[0];
			var data : Texture = context3D.createTexture( source.width, source.height, Context3DTextureFormat.BGRA, false );
			
			for ( var level : int = 0 ; level < texture.sources.length ; level++ )
			{
				source = texture.sources[level];
				
				data.uploadFromBitmapData( source, level );
			}
			
			texture.data = data;
			
			texture_count++;
		}
		
		public function bindTexture( id : int ) : void
		{
			//trace( getTimeStamp() + " bindTexture( " + id + " )" );
			
			tid = id;
			
			if ( tid == 0 ) return;
			
			var texture : Object = textures[tid];
			
			if ( texture )
			{
				texture.time = getTimer();
				
				if ( texture.data == null && texture_count >= 255 )
				{
					dropUnusedTexture();
					reuploadTexture();
				}
			}
			else
			{
				texture = new Object();
				texture.time = getTimer();
				texture.sources = new Array();
				texture.data = null;
				texture.index = tid;
				texture.wrap_mode = MOLEHILL_REPEAT;
				
				textures[tid] = texture;
			}
		}

		public function uploadTexture( level : int, width : int, height : int, data_ptr : uint ) : void
		{
			//trace( getTimeStamp() + " uploadTexture( " + level + ", " + width + ", " + height + ", " + data_ptr + " ) => " + tid );
			
			if ( tid == 0 ) return;

			//if ( width <= 1 || height <= 1 ) return; // prevent the FP11 to crash in OpenGL mode
			
			var texture : Object = textures[tid];
			var source : BitmapData = new BitmapData( width, height, true, 0x00 );
			var rect : Rectangle = new Rectangle( 0, 0, width, height );
			var last_position : uint = ram.position;
			
			ram.position = data_ptr;
			source.setPixels( rect, ram );
			ram.position = last_position;
			
			if ( texture.data == null )
			{
				if ( texture_count >= 255 ) dropUnusedTexture();
				
				texture.data = context3D.createTexture( source.width, source.height, Context3DTextureFormat.BGRA, false );
				
				texture_count++;
				
				//trace( "=============>>>>>>>>>> texture_count=" + texture_count );
			}
			else if ( level == 0 )
			{
				texture.data.dispose();
				texture.data = context3D.createTexture( source.width, source.height, Context3DTextureFormat.BGRA, false );
				
				texture.sources = new Array();
			}
			
			texture.data.uploadFromBitmapData( source, level );
			
			texture.sources[level] = source;
		}
		
		public function setViewport( left : int, top : int, width : int, height : int ) : void
		{
			var fx : Number = width / 640;
			var fy : Number = height / 480;
			
			var ox : Number = left / 320;
			var oy : Number = top / 240;

			viewport_factor = Vector.<Number>( [fx, fy, 1.0, 1.0] );
			viewport_offset = Vector.<Number>( [ox, oy, 0.0, 0.0] );
			
			if ( left == 0 && top == 0 && width == 640 && height == 480 )
				use_viewport_hack = false;
			else
				use_viewport_hack = true;
		}
		
		public function setPolygonOffset( active : Boolean, factor : Number, units : Number ) : void
		{
			//trace( getTimeStamp() + " setPolygonOffset( " + active + " )" );
			
			//use_viewport_hack = active;

			//viewport_factor = Vector.<Number>( [0.5, 0.5, 1.0, 1.0] );
			//viewport_offset = Vector.<Number>( [0.5, 0.5, 0.0, 0.0] );
		}
		
		public function setDepthRange( active : Boolean, near : Number, far : Number ) : void
		{
			//trace( getTimeStamp() + " setDepthRange( " + active + ", " + near + ", " + far + " )" );
			
			if ( near == 0.0 && far == 1.0 )
			{
				use_depthrange_hack = false;

				context3D.setDepthTest( depth_write, depth_func );
			}
			else if ( near == 1.0 && far == 1.0 )
			{
				// disable depth write for sky rendering
				
				use_depthrange_hack = false;

				context3D.setDepthTest( false, Context3DCompareMode.ALWAYS );
			}
			else // if ( near == 0.0 && far == 0.3 )
			{
				// adjust the weapon position on screen
				
				use_depthrange_hack = active;

				depthrange_value = Vector.<Number>( [5.0, -5.0, 0.0, 0.0] );
			}
		}
	}

}