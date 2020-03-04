#ifndef DISPLAY_SERVER_H
#define DISPLAY_SERVER_H

#include "core/callable.h"
#include "core/input/input_filter.h"
#include "core/os/os.h"
#include "core/resource.h"

class DisplayServer : public Object {
	GDCLASS(DisplayServer, Object)

	static DisplayServer *singleton;
	bool vsync_enabled = true;

public:
	_FORCE_INLINE_ static DisplayServer *get_singleton() {
		return singleton;
	}
	enum WindowMode {
		WINDOW_MODE_WINDOWED,
		WINDOW_MODE_MINIMIZED,
		WINDOW_MODE_MAXIMIZED,
		WINDOW_MODE_FULLSCREEN
	};

	typedef DisplayServer *(*CreateFunction)(const String &, WindowMode, uint32_t, const Size2i &, Error &r_error); //video driver, window mode, resolution
	typedef Vector<String> (*GetVideoDriversFunction)(); //video driver, window mode, resolution
private:
	static void _input_set_mouse_mode(InputFilter::MouseMode p_mode);
	static InputFilter::MouseMode _input_get_mouse_mode();
	static void _input_warp(const Vector2 &p_to_pos);
	static InputFilter::CursorShape _input_get_current_cursor_shape();
	static void _input_set_custom_mouse_cursor_func(const RES &, InputFilter::CursorShape, const Vector2 &p_hostspot);

protected:
	static void _bind_methods();
	enum {
		MAX_SERVERS = 64
	};

	struct DisplayServerCreate {
		const char *name;
		GetVideoDriversFunction get_rendering_drivers_function;
		CreateFunction create_function;
	};

	static DisplayServerCreate server_create_functions[MAX_SERVERS];
	static int server_create_count;

	friend class VisualServerRaster;
	virtual void _set_use_vsync(bool p_enable);

public:
	enum Feature {
		FEATURE_GLOBAL_MENU,
		FEATURE_SUBWINDOWS,
		FEATURE_TOUCHSCREEN,
		FEATURE_MOUSE,
		FEATURE_MOUSE_WARP,
		FEATURE_CLIPBOARD,
		FEATURE_VIRTUAL_KEYBOARD,
		FEATURE_CURSOR_SHAPE,
		FEATURE_CUSTOM_CURSOR_SHAPE,
		FEATURE_NATIVE_VIDEO,
		FEATURE_NATIVE_DIALOG,
		FEATURE_CONSOLE_WINDOW,
		FEATURE_IME,
		FEATURE_WINDOW_TRANSPARENCY,
		FEATURE_HIDPI,
		FEATURE_ICON,
		FEATURE_NATIVE_ICON,
		FEATURE_ORIENTATION,
		FEATURE_SWAP_BUFFERS,
		FEATURE_KEEP_SCREEN_ON,

	};

	virtual bool has_feature(Feature p_feature) const = 0;
	virtual String get_name() const = 0;

	virtual void global_menu_add_item(const String &p_menu, const String &p_label, const Variant &p_signal, const Variant &p_meta);
	virtual void global_menu_add_separator(const String &p_menu);
	virtual void global_menu_remove_item(const String &p_menu, int p_idx);
	virtual void global_menu_clear(const String &p_menu);

	virtual void alert(const String &p_alert, const String &p_title = "ALERT!") = 0;

	enum MouseMode {
		MOUSE_MODE_VISIBLE,
		MOUSE_MODE_HIDDEN,
		MOUSE_MODE_CAPTURED,
		MOUSE_MODE_CONFINED
	};

	virtual void mouse_set_mode(MouseMode p_mode);
	virtual MouseMode mouse_get_mode() const;

	virtual void mouse_warp_to_position(const Point2i &p_to);
	virtual Point2i mouse_get_position() const;
	virtual int mouse_get_button_state() const;

	virtual void clipboard_set(const String &p_text);
	virtual String clipboard_get() const;

	enum {
		SCREEN_OF_MAIN_WINDOW = -1
	};

	virtual int get_screen_count() const = 0;
	virtual Point2i screen_get_position(int p_screen = SCREEN_OF_MAIN_WINDOW) const = 0;
	virtual Size2i screen_get_size(int p_screen = SCREEN_OF_MAIN_WINDOW) const = 0;
	virtual int screen_get_dpi(int p_screen = SCREEN_OF_MAIN_WINDOW) const = 0;
	virtual bool screen_is_touchscreen(int p_screen = SCREEN_OF_MAIN_WINDOW) const;
	enum ScreenOrientation {

		SCREEN_LANDSCAPE,
		SCREEN_PORTRAIT,
		SCREEN_REVERSE_LANDSCAPE,
		SCREEN_REVERSE_PORTRAIT,
		SCREEN_SENSOR_LANDSCAPE,
		SCREEN_SENSOR_PORTRAIT,
		SCREEN_SENSOR,
	};

	virtual void screen_set_orientation(ScreenOrientation p_orientation, int p_screen = SCREEN_OF_MAIN_WINDOW);
	ScreenOrientation screen_get_orientation(int p_screen = SCREEN_OF_MAIN_WINDOW) const;

	virtual void screen_set_keep_on(bool p_enable); //disable screensaver
	virtual bool screen_is_kept_on() const;
	enum {
		MAIN_WINDOW_ID = 0,
		INVALID_WINDOW_ID = -1
	};

	typedef int WindowID;

	virtual Vector<int> get_window_list() const = 0;

	enum WindowFlags {
		WINDOW_FLAG_RESIZE_DISABLED,
		WINDOW_FLAG_BORDERLESS,
		WINDOW_FLAG_ALWAYS_ON_TOP,
		WINDOW_FLAG_TRANSPARENT,
		WINDOW_FLAG_MAX,
		WINDOW_FLAG_RESIZE_DISABLED_BIT = (1 << WINDOW_FLAG_RESIZE_DISABLED),
		WINDOW_FLAG_BORDERLESS_BIT = (1 << WINDOW_FLAG_BORDERLESS),
		WINDOW_FLAG_ALWAYS_ON_TOP_BIT = (1 << WINDOW_FLAG_ALWAYS_ON_TOP),
		WINDOW_FLAG_TRANSPARENT_BIT = (1 << WINDOW_FLAG_TRANSPARENT)
	};

	virtual WindowID create_sub_window(WindowMode p_mode, uint32_t p_flags, const Rect2i & = Rect2i());
	virtual void delete_sub_window(WindowID p_id);

	virtual void window_set_resize_callback(const Callable &p_callable, WindowID p_window = MAIN_WINDOW_ID) = 0;

	enum WindowEvent {
		WINDOW_EVENT_MOUSE_ENTER,
		WINDOW_EVENT_MOUSE_EXIT,
		WINDOW_EVENT_FOCUS_IN,
		WINDOW_EVENT_FOCUS_OUT,
		WINDOW_EVENT_CLOSE_REQUEST,
		WINDOW_EVENT_GO_BACK_REQUEST,
	};
	virtual void window_set_window_event_callback(const Callable &p_callable, WindowID p_window = MAIN_WINDOW_ID) = 0;
	virtual void window_set_input_event_callback(const Callable &p_callable, WindowID p_window = MAIN_WINDOW_ID) = 0;
	virtual void window_set_input_text_callback(const Callable &p_callable, WindowID p_window = MAIN_WINDOW_ID) = 0;
	virtual void window_set_drop_files_callback(const Callable &p_callable, WindowID p_window = MAIN_WINDOW_ID) = 0;

	virtual void window_set_title(const String &p_title, WindowID p_window = MAIN_WINDOW_ID) = 0;

	virtual int window_get_current_screen(WindowID p_window = MAIN_WINDOW_ID) const = 0;
	virtual void window_set_current_screen(int p_screen, WindowID p_window = MAIN_WINDOW_ID) = 0;

	virtual Point2i window_get_position(WindowID p_window = MAIN_WINDOW_ID) const = 0;
	virtual void window_set_position(const Point2i &p_position, WindowID p_window = MAIN_WINDOW_ID) = 0;

	virtual void window_set_max_size(const Size2i p_size, WindowID p_window = MAIN_WINDOW_ID) = 0;
	virtual Size2i window_get_max_size(WindowID p_window = MAIN_WINDOW_ID) const = 0;

	virtual void window_set_min_size(const Size2i p_size, WindowID p_window = MAIN_WINDOW_ID) = 0;
	virtual Size2i window_get_min_size(WindowID p_window = MAIN_WINDOW_ID) const = 0;

	virtual void window_set_size(const Size2i p_size, WindowID p_window = MAIN_WINDOW_ID) = 0;
	virtual Size2i window_get_size(WindowID p_window = MAIN_WINDOW_ID) const = 0;
	virtual Size2i window_get_real_size(WindowID p_window = MAIN_WINDOW_ID) const = 0; //wtf is this? should probable use proper name

	virtual void window_set_mode(WindowMode p_mode, WindowID p_window = MAIN_WINDOW_ID) = 0;
	virtual WindowMode window_get_mode(WindowID p_window = MAIN_WINDOW_ID) const = 0;

	virtual bool window_is_maximize_allowed(WindowID p_window = MAIN_WINDOW_ID) const = 0;

	virtual void window_set_flag(WindowFlags p_flag, bool p_enabled, WindowID p_window = MAIN_WINDOW_ID) = 0;
	virtual bool window_get_flag(WindowFlags p_flag, WindowID p_window = MAIN_WINDOW_ID) const = 0;

	virtual void window_request_attention(WindowID p_window = MAIN_WINDOW_ID) = 0;
	virtual void window_move_to_foreground(WindowID p_window = MAIN_WINDOW_ID) = 0;

	virtual bool window_can_draw(WindowID p_window = MAIN_WINDOW_ID) const = 0;

	virtual bool can_any_window_draw() const = 0;

	virtual void window_set_ime_active(const bool p_active, WindowID p_window = MAIN_WINDOW_ID);
	virtual void window_set_ime_position(const Point2i &p_pos, WindowID p_window = MAIN_WINDOW_ID);

	virtual Point2i ime_get_selection() const;
	virtual String ime_get_text() const;

	virtual void console_set_visible(bool p_enabled);
	virtual bool is_console_visible() const;

	virtual void virtual_keyboard_show(const String &p_existing_text, const Rect2 &p_screen_rect = Rect2(), int p_max_legth = -1);
	virtual void virtual_keyboard_hide();

	// returns height of the currently shown virtual keyboard (0 if keyboard is hidden)
	virtual int virtual_keyboard_get_height() const;

	enum CursorShape {
		CURSOR_ARROW,
		CURSOR_IBEAM,
		CURSOR_POINTING_HAND,
		CURSOR_CROSS,
		CURSOR_WAIT,
		CURSOR_BUSY,
		CURSOR_DRAG,
		CURSOR_CAN_DROP,
		CURSOR_FORBIDDEN,
		CURSOR_VSIZE,
		CURSOR_HSIZE,
		CURSOR_BDIAGSIZE,
		CURSOR_FDIAGSIZE,
		CURSOR_MOVE,
		CURSOR_VSPLIT,
		CURSOR_HSPLIT,
		CURSOR_HELP,
		CURSOR_MAX
	};
	virtual void cursor_set_shape(CursorShape p_shape);
	virtual CursorShape cursor_get_shape() const;
	virtual void cursor_set_custom_image(const RES &p_cursor, CursorShape p_shape = CURSOR_ARROW, const Vector2 &p_hotspot = Vector2());

	virtual bool get_swap_ok_cancel();

	virtual void enable_for_stealing_focus(OS::ProcessID pid);

	//plays video natively, in fullscreen, only implemented in mobile for now, likely not possible to implement on linux also.
	virtual Error native_video_play(String p_path, float p_volume, String p_audio_track, String p_subtitle_track, int p_screen = SCREEN_OF_MAIN_WINDOW);
	virtual bool native_video_is_playing() const;
	virtual void native_video_pause();
	virtual void native_video_unpause();
	virtual void native_video_stop();

	virtual Error dialog_show(String p_title, String p_description, Vector<String> p_buttons, const Callable &p_callback);
	virtual Error dialog_input_text(String p_title, String p_description, String p_partial, const Callable &p_callback);

	enum LatinKeyboardVariant {
		LATIN_KEYBOARD_QWERTY,
		LATIN_KEYBOARD_QWERTZ,
		LATIN_KEYBOARD_AZERTY,
		LATIN_KEYBOARD_QZERTY,
		LATIN_KEYBOARD_DVORAK,
		LATIN_KEYBOARD_NEO,
		LATIN_KEYBOARD_COLEMAK,
	};

	virtual LatinKeyboardVariant get_latin_keyboard_variant() const;

	virtual void process_events() = 0;

	virtual void force_process_and_drop_events();

	virtual void release_rendering_thread();
	virtual void make_rendering_thread();
	virtual void swap_buffers();

	virtual void set_native_icon(const String &p_filename);
	virtual void set_icon(const Ref<Image> &p_icon);

	typedef void (*SwitchVSyncCallbackInThread)(bool);

	static SwitchVSyncCallbackInThread switch_vsync_function;

	void vsync_set_enabled(bool p_enable);
	bool vsync_is_enabled() const;

	virtual void vsync_set_use_via_compositor(bool p_enable);
	virtual bool vsync_is_using_via_compositor() const;

	//real, actual overridable function to switch vsync, which needs to be called from graphics thread if needed

	enum Context {
		CONTEXT_EDITOR,
		CONTEXT_PROJECTMAN,
		CONTEXT_ENGINE,
	};

	virtual void set_context(Context p_context);

	static void register_create_function(const char *p_name, CreateFunction p_function, GetVideoDriversFunction p_get_drivers);
	static int get_create_function_count();
	static const char *get_create_function_name(int p_index);
	static Vector<String> get_create_function_rendering_drivers(int p_index);
	static DisplayServer *create(int p_index, const String &p_rendering_driver, WindowMode p_mode, uint32_t p_flags, const Vector2i &p_resolution, Error &r_error);

	DisplayServer();
	~DisplayServer();
};

VARIANT_ENUM_CAST(DisplayServer::WindowEvent)
VARIANT_ENUM_CAST(DisplayServer::Feature)
VARIANT_ENUM_CAST(DisplayServer::MouseMode)
VARIANT_ENUM_CAST(DisplayServer::ScreenOrientation)
VARIANT_ENUM_CAST(DisplayServer::WindowMode)
VARIANT_ENUM_CAST(DisplayServer::WindowFlags)
VARIANT_ENUM_CAST(DisplayServer::CursorShape)
VARIANT_ENUM_CAST(DisplayServer::LatinKeyboardVariant)

#endif // DISPLAY_SERVER_H