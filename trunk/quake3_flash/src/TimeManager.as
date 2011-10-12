package
{
	import flash.display.Stage;
	import flash.events.Event;
	import flash.events.KeyboardEvent;
	import flash.text.TextField;
	import flash.text.TextFormat;
	
	public class TimeManager
	{
		private static const AVERAGE_PERIOD : int = 30; // in seconds
		private static const AVERAGE_DELAY : int = 1000 * (AVERAGE_PERIOD - 1); // in milliseconds
		
		private static var _frame_counters : Vector.<Number>;
		private static var _frame_counters_sum : Number;
		
		private static var _time_offset : Number;
		
		private static var _frame_count : Number;
		private static var _frame_total : Number;
		
		private static var _time_mark : Number;
		private static var _time_total : Number;
		
		private static var _frame_mark : Number;
		private static var _frame_time : Number;
		
		private static var _average_framerate : Number;
		private static var _current_framerate : Number;
		
		private static var _avg_label : TextField;
		private static var _fps_label : TextField;
		
		private static function get externalTime() : Number
		{
			var date : Date = new Date();
			
			return date.getTime();
		}
		
		private static function get internalTime() : Number
		{
			return externalTime - _time_offset;
		}
		
		public static function get frameTime() : Number
		{
			return _frame_time;
		}
		
		public static function get time() : Number
		{
			return _frame_mark;
		}
		
		public static function get averageFramerate() : Number
		{
			return _average_framerate;
		}
		
		public static function get currentFramerate() : Number
		{
			return _current_framerate;
		}
		
		public static function init( stage : Stage ) : void
		{
			_time_offset = externalTime;
			
			_frame_counters = new Vector.<Number>( AVERAGE_PERIOD, true );
			_frame_counters_sum = 0;
			
			_time_mark = 0.0;
			_time_total = 0.0;
			
			_frame_mark = 0.0;
			_frame_time = 0.0;
			
			_frame_count = 0;
			_frame_total = 0;
			
			_average_framerate = 0.0;
			_current_framerate = 0.0;
			
			var format : TextFormat;
			format = new TextFormat;
			format.font = "Courier New";
			format.bold  = true;
			format.size = 16;
			
			_fps_label = new TextField;
			_fps_label.defaultTextFormat = format;
			_fps_label.x = 12;
			_fps_label.y = 12;
			_fps_label.text = "FPS : #";
			_fps_label.width = 180;
			_fps_label.textColor = 0xFFFFFF;
			_fps_label.visible = true;
			_fps_label.selectable = false;
			
			_avg_label = new TextField;
			_avg_label.defaultTextFormat = format;
			_avg_label.x = 12;
			_avg_label.y = 28;
			_avg_label.text = "AVG : #";
			_avg_label.width = 180;
			_avg_label.textColor = 0xFFFFFF;
			_avg_label.visible = true;
			_avg_label.selectable = false;
			
			stage.addChild( _fps_label );
			stage.addChild( _avg_label );
			
			stage.addEventListener( Event.ENTER_FRAME, onEnterFrame );
		}
		
		private static function round( value : Number, precision : Number ) : Number
		{
			var divider : Number = Math.pow( 10, precision );
			
			var result : Number = Math.round( value * divider );
			
			return result / divider;
		}
		
		private static function onEnterFrame( e : Event ) : void
		{
			_frame_count++;
			_frame_total++;
			
			_frame_time = internalTime - _frame_mark;
			_frame_mark = internalTime;
			
			{
				_average_framerate = 1000.0 * (_frame_counters_sum + _frame_count) / (AVERAGE_DELAY + time - _time_mark);
				
				_avg_label.text = "AVG : " + round( _average_framerate, 3 );
				
				if ( _average_framerate > 40 ) _avg_label.textColor = 0xFF00FF00;
				else if ( _average_framerate > 20 ) _avg_label.textColor = 0xFFFFFF00;
				else if ( _average_framerate > 0 ) _avg_label.textColor = 0xFFFF0000;
			}
			
			if ( time - _time_mark >= 1000.0 )
			{
				_current_framerate = 1000.0 * _frame_count / (time - _time_mark);
				
				_frame_counters_sum = 0;
				_frame_counters[0] = _frame_count;
				
				for ( var k : int = AVERAGE_PERIOD - 1 ; k > 0 ; k-- )
				{
					_frame_counters[k] = _frame_counters[k - 1];
					
					_frame_counters_sum += _frame_counters[k];
				}
				
				_time_mark = time;
				_frame_count = 0;
				
				_fps_label.text = "FPS : " + round( _current_framerate, 3 );
				
				if ( _current_framerate > 40 ) _fps_label.textColor = 0xFF00FF00;
				else if ( _current_framerate > 20 ) _fps_label.textColor = 0xFFFFFF00;
				else if ( _current_framerate > 0 ) _fps_label.textColor = 0xFFFF0000;
			}
		}
	}
}