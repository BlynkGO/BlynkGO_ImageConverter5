/* Windows App  */

#include <BlynkGOv5.h>

#define BG_IMGCONV_VERSION        "v5.0.1"

IMAGE_DECLARE(img_blynkgo);

FONT_DECLARE(google_icon_trend_20)
#define SYMBOL_FOLDER             "\ue2c7"
#define SYMBOL_FOLDER_OPEN        "\ue2c8"
#define SYMBOL_VIEW_COMFY         "\ue42a"
#define SYMBOL_RESET_IMAGE        "\uf824"

GContainer cont_frame;
  GContainer cont_title(cont_frame);
    GLabel lb_title("BlynkGO Image Converter " BG_IMGCONV_VERSION, cont_title);
    GLabel lb_sub_title("Convert images to C array format",cont_title);
    GContainer cont_logo(cont_title);
      GImage logo(img_blynkgo, cont_logo);
  GContainer cont_body(cont_frame);
    GTextArea ta_src_path(cont_body);
    GButton btn_src_path(BUTTON_PUSH, "Select File", cont_body);
      GLabel lb_symbol_folder(SYMBOL_FOLDER_OPEN, btn_src_path);
    GContainer cont_conversion(cont_body);
      GLabel lb_conversion_options("Conversion Options", cont_conversion);
      GLabel lb_output_c_filename("Output C Filename", cont_conversion);
      GTextArea ta_output_c_filename(cont_conversion);
      GCheckbox cb_transparent_option(cont_conversion);
      GCheckbox cb_mask_image_option(cont_conversion);

    GLabel  lb_bg_imgconv_status(cont_body);

    GButton btn_convert(BUTTON_PUSH, "Convert!", cont_body);
      GLabel lb_symbol_convert(SYMBOL_RESET_IMAGE, btn_convert);
    

void split_path(const String& full_path, String& folder, String& filename_no_ext, String& filename_with_ext);
void system_run_imgconv(const String& src_full_path, const String& dest_filename_no_ext, String dest_output_folder=".", bool is_transparent=false, bool is_for_masking = false);


void setup(){
  Serial.begin(9600); Serial.println();
  BlynkGO.window_title("BlynkGO Image Converter " BG_IMGCONV_VERSION);

  BlynkGO.begin();
  BlynkGO.fillScreen(TFT_WHITE);
  BlynkGO.show_mouse_info(false);

  cont_frame.size(800, 480);
  cont_frame.GRID_CELL( GRID_COL{ GRID_FR(1)}, GRID_COL { GRID_CONTENT, GRID_FR(1)});
  cont_frame.padding(0);
  cont_frame.bg_opa(0);
  cont_frame.radius(0);

    cont_title.height(90);
    cont_title.grid_cell(0,0, GRID_ALIGN_STRETCH, GRID_ALIGN_START);
    cont_title.color(TFT_COLOR_HEX(0x2563eb));
    cont_title.radius(0);
    cont_title.GRID_CELL(GRID_COL {GRID_FR(1), GRID_CONTENT}, GRID_ROW { 50, 40 });
    cont_title.padding(0,0,20,20,0);
      lb_title.grid_cell(0,0,GRID_ALIGN_START, GRID_ALIGN_END,0, 10);
      lb_title.font(prasanmit_35, TFT_WHITE);
      lb_sub_title.grid_cell(0,1, GRID_ALIGN_START, GRID_ALIGN_START,0,-5);
      lb_sub_title.font(prasanmit_20, TFT_SILVER);
      cont_logo.color(TFT_COLOR_HEX(0x3b82f6));
      cont_logo.padding(10);
      cont_logo.grid_cell(1,0,1,2, GRID_ALIGN_CENTER, GRID_ALIGN_CENTER);
        logo.create();

    cont_body.height(300);
    cont_body.grid_cell(0,1, GRID_ALIGN_STRETCH, GRID_ALIGN_STRETCH);
    cont_body.color(TFT_WHITE);
    cont_body.radius(0);
    cont_body.GRID_CELL( GRID_COL {GRID_FR(1), GRID_CONTENT}, GRID_ROW{40, GRID_CONTENT, GRID_CONTENT, GRID_FR(1)});
    cont_body.padding(20,20,20,20,20);
      ta_src_path.grid_cell(0,0,GRID_ALIGN_STRETCH, GRID_ALIGN_STRETCH);
      ta_src_path.font(prasanmit_20);
      ta_src_path.clickable(false);
      ta_src_path.pad_top(2);
      ta_src_path.pad_bottom(0);
      ta_src_path.placeholder("Click \"Select File\" to choose your image (JPG, PNG, BMP).");
      ta_src_path.font(prasanmit_20, GPART_TEXTAREA_PLACEHOLDER);
      btn_src_path.grid_cell(1,0, GRID_ALIGN_START, GRID_ALIGN_STRETCH);
      btn_src_path.font(prasanmit_20);
      btn_src_path.color(TFT_COLOR_HEX(0x2563eb));
        lb_symbol_folder.font(google_icon_trend_20);
        lb_symbol_folder.layer_id(0);
      btn_src_path.onClicked([](GWIDGET){
        static SoftTimer timer;
        timer.delay(200,[](){
          // กำหนดตัวกรองไฟล์ให้เลือกเฉพาะ .jpg, .png, .bmp
          const char* filters[] = { "*.jpg", "*.png", "*.bmp" };
          // เปิดกล่องเลือกไฟล์
          const char *file = tinyfd_openFileDialog(
            "เลือกไฟล์",  // ชื่อกล่อง
            "",         // ค่าเริ่มต้นของ path
            3,          // จำนวนประเภทไฟล์ที่รองรับ (0 หมายถึงรองรับทุกไฟล์)
            filters,    // ตัวกรองไฟล์ (NULL หมายถึงทุกไฟล์)
            0,          // ตั้งค่าให้เปิดหลายไฟล์หรือไม่ (0 ไม่ได้เปิดหลายไฟล์)
            0           // ตั้งค่าให้แสดงไฟล์ทั้งหมด (0 หมายถึงไม่แสดง)
          );

          if (file) {
            Serial.println(file);
            ta_src_path = file;
            btn_convert.disabled(false);
          } else {
            Serial.println("[FileDialog] no file select");
            ta_src_path = "";
            btn_convert.disabled(true);
          }
        });
      });

      cont_conversion.color(TFT_PALETTE_LIGHTEN(TFT_PALETTE_GRAY,2));//  TFT_COLOR_HEX(0xf9fafb));
      cont_conversion.radius(10);
      cont_conversion.grid_cell(0,1,2,1, GRID_ALIGN_STRETCH, GRID_ALIGN_START);
      cont_conversion.GRID_CELL( GRID_COL { GRID_FR(1)}, GRID_ROW{GRID_CONTENT,GRID_CONTENT,40,8, GRID_CONTENT, GRID_CONTENT});
      cont_conversion.padding(10,10,10,10,0);
        lb_conversion_options.grid_cell(0,0);
        lb_conversion_options.font(prasanmit_25, TFT_BLACK);
        lb_output_c_filename.grid_cell(0,1);
        lb_output_c_filename.font(prasanmit_20, TFT_BLACK);
        ta_output_c_filename.grid_cell(0,2, GRID_ALIGN_STRETCH, GRID_ALIGN_STRETCH);
        ta_output_c_filename.font(prasanmit_20);
        ta_output_c_filename.pad_top(3);
        ta_output_c_filename.pad_bottom(0);
        ta_output_c_filename.placeholder("ตย. img_mypic");
        ta_output_c_filename.default_keyboard(false);
        ta_output_c_filename.cursor_show(true);

        cb_transparent_option.grid_cell(0,4);
        cb_transparent_option.font(prasanmit_20, TFT_COLOR_HSV(0,0,25));
        cb_transparent_option.bg_opa(0);
        cb_transparent_option = "Include transparency (for PNG images)";
        cb_transparent_option.onValueChanged([](GWIDGET){
          if(cb_transparent_option.isON()) {
            cb_mask_image_option.OFF();
          }
        });

        cb_mask_image_option.grid_cell(0,5);
        cb_mask_image_option.font(prasanmit_20, TFT_COLOR_HSV(0,0,25));
        cb_mask_image_option.bg_opa(0);
        cb_mask_image_option = "For masking, supports images with white areas and transparency (e.g., PNG)";
        cb_mask_image_option.margin_top(10);
        cb_mask_image_option.onValueChanged([](GWIDGET){
          if(cb_mask_image_option.isON()) {
            cb_transparent_option.OFF();
          }
        });

      lb_bg_imgconv_status.grid_cell(0,3,2,1, GRID_ALIGN_START, GRID_ALIGN_CENTER);
      lb_bg_imgconv_status.show(false);

      btn_convert.grid_cell(0,3,2,1, GRID_ALIGN_END, GRID_ALIGN_CENTER);
      btn_convert.color(TFT_COLOR_HEX(0x2563eb));
      btn_convert.font(prasanmit_30);
      btn_convert.height(50);
        lb_symbol_convert.font(google_icon_trend_20);
        lb_symbol_convert.layer_id(0);
      btn_convert.disabled(true);
      btn_convert.onClicked([](GWIDGET){
        String full_path = ta_src_path.toString();
        String dest_filename_no_ext = ta_output_c_filename.toString();

        bool is_for_masking = cb_mask_image_option.isON();
        bool is_transparent = cb_transparent_option.isON();

        String folder, filename_no_ext, filename_with_ext;
        split_path(full_path, folder, filename_no_ext, filename_with_ext);

        if( dest_filename_no_ext == "") {
          dest_filename_no_ext = String( is_for_masking? "msk_" : "img_") + filename_no_ext;
        }
        dest_filename_no_ext.replace(".c", "");
        dest_filename_no_ext.replace(" ", "_");
        dest_filename_no_ext.replace("-", "_");

        String dest_filename_with_ext = dest_filename_no_ext + ".c";
        String dest_path = folder + "/" +  dest_filename_with_ext;
      
        // Serial.println("Folder: " + folder);
        // Serial.println("Filename (no ext): " + filename_no_ext);
        // Serial.println("Filename (with ext): " + filename_with_ext);
        // Serial.println("Output Name : " + dest_filename_no_ext);
        // Serial.println("Output : " + dest_path);

        if(is_for_masking){
          system_run_imgconv(full_path, dest_filename_no_ext, folder, false, is_for_masking);
        }else{
          system_run_imgconv(full_path, dest_filename_no_ext, folder, is_transparent, false);
        }

      });

}

void loop() {
  BlynkGO.update();
}

void split_path(const String& full_path, String& folder, String& filename_no_ext, String& filename_with_ext) {
  // แทนที่ \ ด้วย / เพื่อให้ path สามารถใช้งานได้ทั้งใน Windows และ Unix-like
  String path = full_path;
  path.replace("\\", "/");

  // หาตำแหน่งของตัวคั่น path ( / )
  int last_slash = path.lastIndexOf("/");

  // แยก folder
  if (last_slash != -1) {
    folder = path.substring(0, last_slash);
    filename_with_ext = path.substring(last_slash + 1);
  } else {
    folder = "";
    filename_with_ext = path;
  }

  // หาตำแหน่งจุด (.) เพื่อแยกนามสกุลไฟล์
  int last_dot = filename_with_ext.lastIndexOf('.');
  
  if (last_dot != -1) {
    filename_no_ext = filename_with_ext.substring(0, last_dot);
  } else {
    filename_no_ext = filename_with_ext; // ไม่มีนามสกุล
  }
}

void system_run_imgconv(const String& src_full_path, const String& dest_filename_no_ext, String dest_output_folder, bool is_transparent, bool is_for_masking) {

  std::string src     = src_full_path.c_str();
  std::string dest    = dest_filename_no_ext.c_str();
  std::string folder  = dest_output_folder.c_str();

  // สร้างคำสั่งที่ต้องการจะเรียก
  std::string command = "imgconv.exe \"" + src + "\" -o \"" + dest + "\" -of \"" + folder + "\"";
  if ( is_for_masking) {
    command += " -cf A8";
  }else {
    if (is_transparent) command += " -t";
  }

  // String command = "bgimage.exe \"" + src_full_path + "\" --ofmt C --cf RGB565 --output . --outfile \"" + dest_filename_no_ext + "\"";
  // Serial.println(command);
  
  static SoftTimer timer;
  std::string ret = system_silently(command);
  if(ret.find("Done")) {
    lb_bg_imgconv_status = "converted!";
    lb_bg_imgconv_status.color(TFT_PALETTE(TFT_PALETTE_BLUE));
    lb_bg_imgconv_status.show(true);
    timer.delay(5000,[](){
      lb_bg_imgconv_status.show(false);
    });
  }else{
    lb_bg_imgconv_status = "can't convert!";
    lb_bg_imgconv_status.color(TFT_PALETTE(TFT_PALETTE_RED));
    lb_bg_imgconv_status.show(true);
    timer.delay(5000,[](){
      lb_bg_imgconv_status.show(false);
    });

  }
}

