// 本文件由 tools/attribute_gen.lua 生成，请勿手改。
// 数据来源：tools/attribute_defs.lua（新增属性请改数据表后运行 xmake attribute-gen）

#include "duilib/Utils/AttributeIds.g.h"
#include "duilib/duilib_config.h"

#include <unordered_set>

namespace ui {
namespace attr {
namespace {
/** 运行期 FNV-1a 32 位哈希，必须与生成器 fnv1a 的实现完全一致。
*/
uint32_t Hash(const DString &strName)
{
    uint32_t h = 2166136261u;
    for (DString::value_type c : strName) {
        h ^= (uint32_t) c;
        h *= 16777619u;
    }
    return h;
}
} //namespace

namespace control {
Id IdOf(const DString &strName)
{
    switch (Hash(strName)) {
    case 0xBCBD1237u: //multi_select
        return Id::kMultiSelect;
    case 0x4D85F8DEu: //paint_selected_colors
        return Id::kPaintSelectedColors;
    case 0xFBE00531u: //scroll_select
        return Id::kScrollSelect;
    case 0xD2056F94u: //scrollselect（别名，归一到 scroll_select）
        return Id::kScrollSelect;
    case 0x50AE2588u: //select_next_when_active_removed
        return Id::kSelectNextWhenActiveRemoved;
    case 0xFF458ECFu: //frame_selection
        return Id::kFrameSelection;
    case 0x7993C147u: //frame_selection_color
        return Id::kFrameSelectionColor;
    case 0xA90E6004u: //frame_selection_alpha
        return Id::kFrameSelectionAlpha;
    case 0xF33D4062u: //frame_selection_border_size
        return Id::kFrameSelectionBorderSize;
    case 0x5AB04442u: //frame_selection_border_color
        return Id::kFrameSelectionBorderColor;
    case 0xA380B92Fu: //select_none_when_click_blank
        return Id::kSelectNoneWhenClickBlank;
    case 0xD21E5056u: //select_like_list_ctrl
        return Id::kSelectLikeListCtrl;
    case 0xF4FAB7EDu: //vscrollbar
        return Id::kVscrollbar;
    case 0x8F685DFBu: //vscrollbar_style
        return Id::kVscrollbarStyle;
    case 0x7F5A74AEu: //vscrollbarstyle（别名，归一到 vscrollbar_style）
        return Id::kVscrollbarStyle;
    case 0x85B8ACF6u: //vscrollbar_class
        return Id::kVscrollbarClass;
    case 0x4101035Fu: //hscrollbar
        return Id::kHscrollbar;
    case 0x9BDA40E5u: //hscrollbar_style
        return Id::kHscrollbarStyle;
    case 0xC2C03080u: //hscrollbarstyle（别名，归一到 hscrollbar_style）
        return Id::kHscrollbarStyle;
    case 0x72013C18u: //hscrollbar_class
        return Id::kHscrollbarClass;
    case 0xCA79D1EBu: //scrollbar_padding
        return Id::kScrollbarPadding;
    case 0x9E9FF992u: //scrollbarpadding（别名，归一到 scrollbar_padding）
        return Id::kScrollbarPadding;
    case 0xFB589697u: //vscroll_unit
        return Id::kVscrollUnit;
    case 0x6DC07916u: //vscrollunit（别名，归一到 vscroll_unit）
        return Id::kVscrollUnit;
    case 0x245D5151u: //hscroll_unit
        return Id::kHscrollUnit;
    case 0xB9FDCCF0u: //hscrollunit（别名，归一到 hscroll_unit）
        return Id::kHscrollUnit;
    case 0xE2A526B0u: //scrollbar_float
        return Id::kScrollbarFloat;
    case 0xB2A4D241u: //scrollbarfloat（别名，归一到 scrollbar_float）
        return Id::kScrollbarFloat;
    case 0xFC61EC73u: //vscrollbar_left
        return Id::kVscrollbarLeft;
    case 0xDCE83188u: //vscrollbarleft（别名，归一到 vscrollbar_left）
        return Id::kVscrollbarLeft;
    case 0xD4367E0Au: //hold_end
        return Id::kHoldEnd;
    case 0xA06270DDu: //holdend（别名，归一到 hold_end）
        return Id::kHoldEnd;
    case 0x542B1E04u: //selected_id
        return Id::kSelectedId;
    case 0x696178F9u: //selectedid（别名，归一到 selected_id）
        return Id::kSelectedId;
    case 0xEE437CF0u: //fade_switch
        return Id::kFadeSwitch;
    case 0xB7803243u: //fadeswitch（别名，归一到 fade_switch）
        return Id::kFadeSwitch;
    case 0xEC051ED5u: //fade_switch_type
        return Id::kFadeSwitchType;
    case 0x8A3E46DFu: //fade_switch_frame_interval_ms
        return Id::kFadeSwitchFrameIntervalMs;
    case 0xD240487Eu: //fade_switch_total_ms
        return Id::kFadeSwitchTotalMs;
    case 0x7F8844D1u: //fade_switch_easing_function
        return Id::kFadeSwitchEasingFunction;
    case 0x9EFC33E5u: //xml_file_path
        return Id::kXmlFilePath;
    case 0x8763D351u: //res_path
        return Id::kResPath;
    case 0x901089D3u: //address_path
        return Id::kAddressPath;
    case 0x8E6CB50Au: //path_tooltip
        return Id::kPathTooltip;
    case 0xB852D81Cu: //return_update_ui
        return Id::kReturnUpdateUi;
    case 0x21D5447Du: //esc_update_ui
        return Id::kEscUpdateUi;
    case 0x59C910B5u: //kill_focus_update_ui
        return Id::kKillFocusUpdateUi;
    case 0xD4B31263u: //rich_edit_class
        return Id::kRichEditClass;
    case 0xFF9E965Au: //rich_edit_clear_btn_class
        return Id::kRichEditClearBtnClass;
    case 0x7457FF18u: //sub_path_hbox_class
        return Id::kSubPathHboxClass;
    case 0x60EBBAA5u: //sub_path_button_class
        return Id::kSubPathButtonClass;
    case 0x3BCECAB1u: //sub_path_root_class
        return Id::kSubPathRootClass;
    case 0x0FEEAB5Du: //path_separator_class
        return Id::kPathSeparatorClass;
    case 0x322B6482u: //bitmap_halign
        return Id::kBitmapHalign;
    case 0x113E6CDCu: //bitmap_valign
        return Id::kBitmapValign;
    case 0x7D7F3C8Du: //bitmap_alpha
        return Id::kBitmapAlpha;
    case 0xBE5F453Fu: //bitmap_dest
        return Id::kBitmapDest;
    case 0x1FE4EC07u: //bitmap_src
        return Id::kBitmapSrc;
    case 0xF572B14Du: //bitmap_margin
        return Id::kBitmapMargin;
    case 0x6B230B2Du: //bitmap_adaptive_dest_rect
        return Id::kBitmapAdaptiveDestRect;
    case 0x1D82C818u: //bitmap_stretch
        return Id::kBitmapStretch;
    case 0xD5F06CADu: //bitmap_multi_thread
        return Id::kBitmapMultiThread;
    case 0xB2390415u: //bitmap_file
        return Id::kBitmapFile;
    case 0xE7D55925u: //dropbox
        return Id::kDropbox;
    case 0xB20682F6u: //dropbox_item_class
        return Id::kDropboxItemClass;
    case 0x8D03ED31u: //selected_item_class
        return Id::kSelectedItemClass;
    case 0xE4B3054Fu: //dropbox_size
        return Id::kDropboxSize;
    case 0x3F2A35BCu: //dropboxsize（别名，归一到 dropbox_size）
        return Id::kDropboxSize;
    case 0xB75EC2D1u: //popup_top
        return Id::kPopupTop;
    case 0xBF0F3414u: //popuptop（别名，归一到 popup_top）
        return Id::kPopupTop;
    case 0xD5BDBB42u: //height
        return Id::kHeight;
    case 0xB1B4685Au: //shadow_type
        return Id::kShadowType;
    case 0xB8649EE9u: //child_window_margin
        return Id::kChildWindowMargin;
    case 0x026723E0u: //circular
        return Id::kCircular;
    case 0x9C2D035Eu: //circle_width
        return Id::kCircleWidth;
    case 0x8907F8E3u: //circlewidth（别名，归一到 circle_width）
        return Id::kCircleWidth;
    case 0x04AD815Eu: //indicator
        return Id::kIndicator;
    case 0x53980915u: //clockwise
        return Id::kClockwise;
    case 0xCC4A7989u: //bgcolor
        return Id::kBgcolor;
    case 0x6EBE112Du: //fgcolor
        return Id::kFgcolor;
    case 0xAA23AEF1u: //gradient_color
        return Id::kGradientColor;
    case 0x686EA4B8u: //gradientcolor（别名，归一到 gradient_color）
        return Id::kGradientColor;
    case 0x93DC7806u: //cursor_file
        return Id::kCursorFile;
    case 0xE821108Du: //color_type
        return Id::kColorType;
    case 0x2520F4BAu: //combo_type
        return Id::kComboType;
    case 0x207D1CA5u: //combo_tree_view_class
        return Id::kComboTreeViewClass;
    case 0x039DA774u: //combo_tree_node_class
        return Id::kComboTreeNodeClass;
    case 0x4DC30B50u: //combo_icon_class
        return Id::kComboIconClass;
    case 0x5182275Du: //combo_edit_class
        return Id::kComboEditClass;
    case 0x8DC12689u: //combo_button_class
        return Id::kComboButtonClass;
    case 0xF9E45CC0u: //combo_box_class
        return Id::kComboBoxClass;
    case 0x675317B2u: //left_button_class
        return Id::kLeftButtonClass;
    case 0x8B37C7ABu: //left_button_top_label_class
        return Id::kLeftButtonTopLabelClass;
    case 0x088051D7u: //left_button_bottom_label_class
        return Id::kLeftButtonBottomLabelClass;
    case 0x6C89B1B2u: //left_button_top_label_text
        return Id::kLeftButtonTopLabelText;
    case 0x47D04F46u: //left_button_bottom_label_text
        return Id::kLeftButtonBottomLabelText;
    case 0x7B469659u: //left_button_top_label_bkcolor
        return Id::kLeftButtonTopLabelBkcolor;
    case 0xC4614905u: //left_button_bottom_label_bkcolor
        return Id::kLeftButtonBottomLabelBkcolor;
    case 0x22D6CDB5u: //right_button_class
        return Id::kRightButtonClass;
    case 0xB99D8552u: //format
        return Id::kFormat;
    case 0xAD4E22A9u: //edit_format
        return Id::kEditFormat;
    case 0xAA9BE722u: //spin_class
        return Id::kSpinClass;
    case 0x72A219D0u: //small_icon_size
        return Id::kSmallIconSize;
    case 0x46699108u: //large_icon_size
        return Id::kLargeIconSize;
    case 0x0A0ECF59u: //show_hiden_files
        return Id::kShowHidenFiles;
    case 0xEBCC3FB0u: //show_system_files
        return Id::kShowSystemFiles;
    case 0x88849354u: //default_text
        return Id::kDefaultText;
    case 0xE54C09A4u: //default_text_id
        return Id::kDefaultTextId;
    case 0x4B388A5Cu: //ip
        return Id::kIp;
    case 0x1ADCC598u: //text_align
        return Id::kTextAlign;
    case 0xA5C431FCu: //end_ellipsis
        return Id::kEndEllipsis;
    case 0xA3D09645u: //endellipsis（别名，归一到 end_ellipsis）
        return Id::kEndEllipsis;
    case 0xC0B426D0u: //path_ellipsis
        return Id::kPathEllipsis;
    case 0x84448139u: //pathellipsis（别名，归一到 path_ellipsis）
        return Id::kPathEllipsis;
    case 0x0A74C270u: //single_line
        return Id::kSingleLine;
    case 0xD5A2F4B3u: //singleline（别名，归一到 single_line）
        return Id::kSingleLine;
    case 0xAA937CA5u: //multi_line
        return Id::kMultiLine;
    case 0x2419AFA0u: //multiline（别名，归一到 multi_line）
        return Id::kMultiLine;
    case 0xBDE64E3Eu: //text
        return Id::kText;
    case 0x7ACD9B7Eu: //text_id
        return Id::kTextId;
    case 0xC778FA43u: //textid（别名，归一到 text_id）
        return Id::kTextId;
    case 0xA2E409AAu: //auto_tooltip
        return Id::kAutoTooltip;
    case 0xA47B3F0Du: //autotooltip（别名，归一到 auto_tooltip）
        return Id::kAutoTooltip;
    case 0x274E1290u: //font
        return Id::kFont;
    case 0xA74C1692u: //text_color
        return Id::kTextColor;
    case 0x95591870u: //normal_text_color
        return Id::kNormalTextColor;
    case 0x2BC4C61Cu: //normaltextcolor（别名，归一到 normal_text_color）
        return Id::kNormalTextColor;
    case 0xEFFBEBB6u: //hovered_text_color
        return Id::kHoveredTextColor;
    case 0xAC86A296u: //hot_text_color
        return Id::kHotTextColor;
    case 0x49ED93A2u: //hottextcolor（别名，归一到 hot_text_color）
        return Id::kHotTextColor;
    case 0x3421EAF9u: //pressed_text_color
        return Id::kPressedTextColor;
    case 0x8E9ACB86u: //pushed_text_color
        return Id::kPushedTextColor;
    case 0xF2C58E52u: //pushedtextcolor（别名，归一到 pushed_text_color）
        return Id::kPushedTextColor;
    case 0x732B9F59u: //disabled_text_color
        return Id::kDisabledTextColor;
    case 0x04E4DBE1u: //disabledtextcolor（别名，归一到 disabled_text_color）
        return Id::kDisabledTextColor;
    case 0x5E9F4868u: //text_padding
        return Id::kTextPadding;
    case 0xD4CBDF2Fu: //textpadding（别名，归一到 text_padding）
        return Id::kTextPadding;
    case 0xCF3B0DE0u: //replace_newline
        return Id::kReplaceNewline;
    case 0xF4132023u: //spacing_mul
        return Id::kSpacingMul;
    case 0xD2B5E896u: //spacing_add
        return Id::kSpacingAdd;
    case 0xC60AD5ABu: //vertical_text
        return Id::kVerticalText;
    case 0xEB9FED5Bu: //word_spacing
        return Id::kWordSpacing;
    case 0x50A0196Eu: //use_font_height
        return Id::kUseFontHeight;
    case 0xB8CF0CEEu: //ascii_rotate_90
        return Id::kAsciiRotate90;
    case 0x1E1F1AAFu: //rich_text
        return Id::kRichText;
    case 0x8729D53Bu: //vertical
        return Id::kVertical;
    case 0xD944F83Fu: //line_color
        return Id::kLineColor;
    case 0x70A37EC4u: //line_width
        return Id::kLineWidth;
    case 0x0964C593u: //dash_style
        return Id::kDashStyle;
    case 0xB70C9F07u: //header_class
        return Id::kHeaderClass;
    case 0xE6FBF0F5u: //header_item_class
        return Id::kHeaderItemClass;
    case 0xA7C8CF0Eu: //header_split_box_class
        return Id::kHeaderSplitBoxClass;
    case 0x84BE7DDAu: //header_split_control_class
        return Id::kHeaderSplitControlClass;
    case 0x62B44966u: //enable_header_drag_order
        return Id::kEnableHeaderDragOrder;
    case 0x0E1D474Au: //check_box_class
        return Id::kCheckBoxClass;
    case 0x41B2DB76u: //data_item_class
        return Id::kDataItemClass;
    case 0x1F3DA5B9u: //data_sub_item_class
        return Id::kDataSubItemClass;
    case 0x298731B0u: //row_grid_line_width
        return Id::kRowGridLineWidth;
    case 0xD62D00FBu: //row_grid_line_color
        return Id::kRowGridLineColor;
    case 0x4D4F9E14u: //column_grid_line_width
        return Id::kColumnGridLineWidth;
    case 0x9F2233EFu: //column_grid_line_color
        return Id::kColumnGridLineColor;
    case 0xE4D8604Eu: //report_view_class
        return Id::kReportViewClass;
    case 0xAB8A5E5Au: //header_height
        return Id::kHeaderHeight;
    case 0xF89A309Du: //data_item_height
        return Id::kDataItemHeight;
    case 0xCF8E576Cu: //show_header
        return Id::kShowHeader;
    case 0x95B686C8u: //enable_column_width_auto
        return Id::kEnableColumnWidthAuto;
    case 0xE907133Eu: //auto_check_select
        return Id::kAutoCheckSelect;
    case 0x3C7BF20Cu: //show_header_checkbox
        return Id::kShowHeaderCheckbox;
    case 0x4546F92Fu: //show_data_item_checkbox
        return Id::kShowDataItemCheckbox;
    case 0x5127F14Du: //type
        return Id::kType;
    case 0x230D75A7u: //icon_view_class
        return Id::kIconViewClass;
    case 0x4FE96A55u: //icon_view_item_class
        return Id::kIconViewItemClass;
    case 0x34D8BC1Fu: //icon_view_item_image_class
        return Id::kIconViewItemImageClass;
    case 0xD0A6B804u: //icon_view_item_label_class
        return Id::kIconViewItemLabelClass;
    case 0x672E884Cu: //list_view_class
        return Id::kListViewClass;
    case 0xC93BF194u: //list_view_item_class
        return Id::kListViewItemClass;
    case 0x4730B7D2u: //list_view_item_image_class
        return Id::kListViewItemImageClass;
    case 0xCA05BF61u: //list_view_item_label_class
        return Id::kListViewItemLabelClass;
    case 0xB980CC95u: //enable_item_edit
        return Id::kEnableItemEdit;
    case 0x04A8920Du: //list_ctrl_richedit_class
        return Id::kListCtrlRicheditClass;
    case 0x9188CDC6u: //icon_spacing
        return Id::kIconSpacing;
    case 0x59886E22u: //sorted_up_image
        return Id::kSortedUpImage;
    case 0x307AA303u: //sorted_down_image
        return Id::kSortedDownImage;
    case 0xABE5B538u: //show_icon_at_top
        return Id::kShowIconAtTop;
    case 0x26C4B19Cu: //horizontal_layout
        return Id::kHorizontalLayout;
    case 0x55358A69u: //horizontal
        return Id::kHorizontal;
    case 0xFCC3A4AEu: //hor
        return Id::kHor;
    case 0xC98F4557u: //min
        return Id::kMin;
    case 0xD7A2E319u: //max
        return Id::kMax;
    case 0x425ED3CAu: //value
        return Id::kValue;
    case 0x56BC98A0u: //progress_image
        return Id::kProgressImage;
    case 0xCC819F2Bu: //progressimage（别名，归一到 progress_image）
        return Id::kProgressImage;
    case 0x211FCDBBu: //stretch_fore_image
        return Id::kStretchForeImage;
    case 0x4DF158A2u: //is_stretch_fore
        return Id::kIsStretchFore;
    case 0xC3634E44u: //isstretchfore（别名，归一到 is_stretch_fore）
        return Id::kIsStretchFore;
    case 0x1CD8603Au: //progress_color
        return Id::kProgressColor;
    case 0x395FB3A9u: //progresscolor（别名，归一到 progress_color）
        return Id::kProgressColor;
    case 0x02ACE84Bu: //marquee
        return Id::kMarquee;
    case 0x3171E7C8u: //marquee_width
        return Id::kMarqueeWidth;
    case 0x2667884Du: //marqueewidth（别名，归一到 marquee_width）
        return Id::kMarqueeWidth;
    case 0xC67C576Eu: //marquee_step
        return Id::kMarqueeStep;
    case 0xE8C72E29u: //marqueestep（别名，归一到 marquee_step）
        return Id::kMarqueeStep;
    case 0x21506C05u: //reverse
        return Id::kReverse;
    case 0x33D51DE7u: //property_grid_xml
        return Id::kPropertyGridXml;
    case 0xA89AB6B3u: //group_class
        return Id::kGroupClass;
    case 0x3EADB08Au: //group_label_class
        return Id::kGroupLabelClass;
    case 0x6222EA4Fu: //property_class
        return Id::kPropertyClass;
    case 0x7358B17Cu: //property_name_label_class
        return Id::kPropertyNameLabelClass;
    case 0x2C6FA660u: //property_value_label_class
        return Id::kPropertyValueLabelClass;
    case 0x796E820Cu: //left_column_width
        return Id::kLeftColumnWidth;
    case 0x3E64A9FAu: //property_font_normal
        return Id::kPropertyFontNormal;
    case 0xAEAAA55Au: //property_font_modified
        return Id::kPropertyFontModified;
    case 0xCE0BEFF7u: //readonly
        return Id::kReadonly;
    case 0x364B5F18u: //password
        return Id::kPassword;
    case 0x55B6F044u: //show_password
        return Id::kShowPassword;
    case 0x30DD521Du: //password_char
        return Id::kPasswordChar;
    case 0x5811D3ACu: //flash_password_char
        return Id::kFlashPasswordChar;
    case 0x66D3B1BFu: //number_only
        return Id::kNumberOnly;
    case 0x1BD670A0u: //number
        return Id::kNumber;
    case 0x04BA38FFu: //max_number
        return Id::kMaxNumber;
    case 0x6EFEDCC9u: //min_number
        return Id::kMinNumber;
    case 0x307F74BAu: //number_format
        return Id::kNumberFormat;
    case 0x2882C428u: //caret_color
        return Id::kCaretColor;
    case 0xCF4CF1FFu: //caretcolor（别名，归一到 caret_color）
        return Id::kCaretColor;
    case 0xBA66A50Fu: //prompt_mode
        return Id::kPromptMode;
    case 0xEC8D1D64u: //promptmode（别名，归一到 prompt_mode）
        return Id::kPromptMode;
    case 0x3DF6AA33u: //prompt_color
        return Id::kPromptColor;
    case 0x93068D9Au: //promptcolor（别名，归一到 prompt_color）
        return Id::kPromptColor;
    case 0x4DC331ABu: //prompt_text
        return Id::kPromptText;
    case 0x40F3DBCCu: //prompttext（别名，归一到 prompt_text）
        return Id::kPromptText;
    case 0x9031F911u: //prompt_text_id
        return Id::kPromptTextId;
    case 0xB6493F26u: //prompt_textid
        return Id::kPromptTextid;
    case 0x2BFBE4C1u: //prompttextid
        return Id::kPrompttextid;
    case 0x4F46186Cu: //focused_image
        return Id::kFocusedImage;
    case 0x794F10B7u: //focusedimage（别名，归一到 focused_image）
        return Id::kFocusedImage;
    case 0x4A22DE4Fu: //want_tab
        return Id::kWantTab;
    case 0xF37B5B26u: //wanttab（别名，归一到 want_tab）
        return Id::kWantTab;
    case 0x05AF06F6u: //want_return
        return Id::kWantReturn;
    case 0xA014BE3Cu: //want_return_msg
        return Id::kWantReturnMsg;
    case 0xE64E72CAu: //wantreturnmsg（别名，归一到 want_return_msg）
        return Id::kWantReturnMsg;
    case 0xE0150E5Cu: //want_ctrl_return
        return Id::kWantCtrlReturn;
    case 0x1BD4273Cu: //return_msg_want_ctrl
        return Id::kReturnMsgWantCtrl;
    case 0x1A728717u: //returnmsgwantctrl（别名，归一到 return_msg_want_ctrl）
        return Id::kReturnMsgWantCtrl;
    case 0x7F900142u: //limit_text
        return Id::kLimitText;
    case 0xC2E9BC66u: //max_char
        return Id::kMaxChar;
    case 0x1902D821u: //maxchar（别名，归一到 max_char）
        return Id::kMaxChar;
    case 0xD9D7502Eu: //limit_chars
        return Id::kLimitChars;
    case 0x363BBB42u: //word_wrap
        return Id::kWordWrap;
    case 0x3166CF21u: //no_caret_readonly
        return Id::kNoCaretReadonly;
    case 0x50A989B0u: //default_context_menu
        return Id::kDefaultContextMenu;
    case 0x4AD59076u: //clear_btn_class
        return Id::kClearBtnClass;
    case 0x3A02E8F4u: //show_password_btn_class
        return Id::kShowPasswordBtnClass;
    case 0xD730128Eu: //wheel_zoom
        return Id::kWheelZoom;
    case 0x58CF1E70u: //hide_selection
        return Id::kHideSelection;
    case 0xAA940D59u: //focused_bottom_border_size
        return Id::kFocusedBottomBorderSize;
    case 0x54F29082u: //focus_bottom_border_size
        return Id::kFocusBottomBorderSize;
    case 0x86B2165Fu: //focused_bottom_border_color
        return Id::kFocusedBottomBorderColor;
    case 0x0B1D01E2u: //focus_bottom_border_color
        return Id::kFocusBottomBorderColor;
    case 0xDF92E232u: //zoom
        return Id::kZoom;
    case 0xF3BD77D6u: //auto_vscroll
        return Id::kAutoVscroll;
    case 0x674175F1u: //autovscroll（别名，归一到 auto_vscroll）
        return Id::kAutoVscroll;
    case 0x4C91E398u: //auto_hscroll
        return Id::kAutoHscroll;
    case 0x23AB8B43u: //autohscroll（别名，归一到 auto_hscroll）
        return Id::kAutoHscroll;
    case 0x1252D727u: //rich
        return Id::kRich;
    case 0x8C6123CAu: //auto_detect_url
        return Id::kAutoDetectUrl;
    case 0xD3C3F0E3u: //allow_beep
        return Id::kAllowBeep;
    case 0x1995BD25u: //save_selection
        return Id::kSaveSelection;
    case 0x725BC851u: //select_all_on_focused
        return Id::kSelectAllOnFocused;
    case 0x99E2DE50u: //select_all_on_focus
        return Id::kSelectAllOnFocus;
    case 0x4E1F4E80u: //selection_bkcolor
        return Id::kSelectionBkcolor;
    case 0xE9C57CC0u: //inactive_selection_bkcolor
        return Id::kInactiveSelectionBkcolor;
    case 0xE1D14D28u: //current_row_bkcolor
        return Id::kCurrentRowBkcolor;
    case 0x1678E068u: //inactive_current_row_bkcolor
        return Id::kInactiveCurrentRowBkcolor;
    case 0x18258208u: //row_spacing_mul
        return Id::kRowSpacingMul;
    case 0x1EE11B5Du: //row_spacing_add
        return Id::kRowSpacingAdd;
    case 0x73EB0CCEu: //enable_drag_out
        return Id::kEnableDragOut;
    case 0x14512AB1u: //default_link_font_color
        return Id::kDefaultLinkFontColor;
    case 0xEBA3E847u: //hovered_link_font_color
        return Id::kHoveredLinkFontColor;
    case 0x1A9C2C3Eu: //hover_link_font_color
        return Id::kHoverLinkFontColor;
    case 0xFAF06F76u: //pressed_link_font_color
        return Id::kPressedLinkFontColor;
    case 0xFD3068E2u: //mouse_down_link_font_color
        return Id::kMouseDownLinkFontColor;
    case 0x348509B0u: //link_font_underline
        return Id::kLinkFontUnderline;
    case 0xE019C0FFu: //replace_brace
        return Id::kReplaceBrace;
    case 0xBDF2EC04u: //trim_policy
        return Id::kTrimPolicy;
    case 0xC7441A0Fu: //step
        return Id::kStep;
    case 0x90C00B85u: //thumb_normal_image
        return Id::kThumbNormalImage;
    case 0x9EFD80A9u: //thumbnormalimage（别名，归一到 thumb_normal_image）
        return Id::kThumbNormalImage;
    case 0x47791675u: //thumb_hovered_image
        return Id::kThumbHoveredImage;
    case 0x7B437A2Du: //thumb_hot_image
        return Id::kThumbHotImage;
    case 0xF6A6593Fu: //thumbhotimage（别名，归一到 thumb_hot_image）
        return Id::kThumbHotImage;
    case 0x821AD674u: //thumb_pressed_image
        return Id::kThumbPressedImage;
    case 0x62A5F5DFu: //thumb_pushed_image
        return Id::kThumbPushedImage;
    case 0xDF729DFBu: //thumbpushedimage（别名，归一到 thumb_pushed_image）
        return Id::kThumbPushedImage;
    case 0x96264A7Eu: //thumb_disabled_image
        return Id::kThumbDisabledImage;
    case 0xBBEC1628u: //thumbdisabledimage（别名，归一到 thumb_disabled_image）
        return Id::kThumbDisabledImage;
    case 0x023175ADu: //thumb_size
        return Id::kThumbSize;
    case 0xD3E01A62u: //thumbsize（别名，归一到 thumb_size）
        return Id::kThumbSize;
    case 0xDC01E67Au: //progress_bar_padding
        return Id::kProgressBarPadding;
    case 0x8259FCDCu: //progressbarpadding（别名，归一到 progress_bar_padding）
        return Id::kProgressBarPadding;
    case 0x8C219EC0u: //tab_box_name
        return Id::kTabBoxName;
    case 0xC9D18F6Cu: //drag_order
        return Id::kDragOrder;
    case 0x7DBB2D3Cu: //selected_tab_item_outline_width
        return Id::kSelectedTabItemOutlineWidth;
    case 0x6BCF4D07u: //selected_tab_item_outline_color
        return Id::kSelectedTabItemOutlineColor;
    case 0xF7C1743Du: //tab_ctrl_bottom_line_height
        return Id::kTabCtrlBottomLineHeight;
    case 0xEBEB1DB9u: //tab_ctrl_bottom_line_color
        return Id::kTabCtrlBottomLineColor;
    case 0xE509C28Fu: //tab_box_item_index
        return Id::kTabBoxItemIndex;
    case 0x9865B509u: //title
        return Id::kTitle;
    case 0x7C037C6Fu: //title_id
        return Id::kTitleId;
    case 0xE64015F0u: //icon
        return Id::kIcon;
    case 0x8AAC1AF7u: //icon_class
        return Id::kIconClass;
    case 0x14911EB2u: //title_class
        return Id::kTitleClass;
    case 0x037FF977u: //close_button_class
        return Id::kCloseButtonClass;
    case 0x5DC50740u: //line_class
        return Id::kLineClass;
    case 0xA465B32Du: //selected_round_corner
        return Id::kSelectedRoundCorner;
    case 0x53A73A2Du: //hovered_round_corner
        return Id::kHoveredRoundCorner;
    case 0x900BD28Du: //hot_round_corner
        return Id::kHotRoundCorner;
    case 0xA09CD5DAu: //hovered_padding
        return Id::kHoveredPadding;
    case 0x0A65AC7Au: //hot_padding
        return Id::kHotPadding;
    case 0x79C9DE29u: //auto_hide_close_button
        return Id::kAutoHideCloseButton;
    case 0x6FA649E7u: //expand_normal_image
        return Id::kExpandNormalImage;
    case 0x5416DAEBu: //expand_hovered_image
        return Id::kExpandHoveredImage;
    case 0x256E1AEBu: //expand_hot_image
        return Id::kExpandHotImage;
    case 0x88F0DA32u: //expand_pressed_image
        return Id::kExpandPressedImage;
    case 0xE9B950E9u: //expand_pushed_image
        return Id::kExpandPushedImage;
    case 0xB34F8E0Cu: //expand_disabled_image
        return Id::kExpandDisabledImage;
    case 0x42DFA09Eu: //collapse_normal_image
        return Id::kCollapseNormalImage;
    case 0xB27B1E1Cu: //collapse_hovered_image
        return Id::kCollapseHoveredImage;
    case 0xFA22C004u: //collapse_hot_image
        return Id::kCollapseHotImage;
    case 0x8F40D689u: //collapse_pressed_image
        return Id::kCollapsePressedImage;
    case 0x8298E7A0u: //collapse_pushed_image
        return Id::kCollapsePushedImage;
    case 0x2DD1B7E1u: //collapse_disabled_image
        return Id::kCollapseDisabledImage;
    case 0xEB0699F3u: //expand_image_right_space
        return Id::kExpandImageRightSpace;
    case 0x38B83753u: //check_box_image_right_space
        return Id::kCheckBoxImageRightSpace;
    case 0x99BDF6CAu: //icon_image_right_space
        return Id::kIconImageRightSpace;
    case 0x16FF7DBBu: //indent
        return Id::kIndent;
    case 0x6FF656E8u: //expand_image_class
        return Id::kExpandImageClass;
    case 0x572841B4u: //show_icon
        return Id::kShowIcon;
    case 0x4606CAB1u: //mouse_child
        return Id::kMouseChild;
    case 0xBF5DE7F2u: //mousechild（别名，归一到 mouse_child）
        return Id::kMouseChild;
    case 0x6C5106A0u: //drag_out_id
        return Id::kDragOutId;
    case 0x460FA622u: //drop_in_id
        return Id::kDropInId;
    case 0xAB3E0BFFu: //class
        return Id::kClass;
    case 0x546F8B01u: //enable_vars
        return Id::kEnableVars;
    case 0x4A28CB3Cu: //halign
        return Id::kHalign;
    case 0x5A9DCFE2u: //valign
        return Id::kValign;
    case 0x602C63DEu: //align
        return Id::kAlign;
    case 0xD7713C7Bu: //margin
        return Id::kMargin;
    case 0x809608B6u: //padding
        return Id::kPadding;
    case 0x65E1AA08u: //control_padding
        return Id::kControlPadding;
    case 0x5A238A9Du: //bkcolor
        return Id::kBkcolor;
    case 0x94F3517Du: //bkcolor2
        return Id::kBkcolor2;
    case 0xC5299E5Bu: //bkcolor2_direction
        return Id::kBkcolor2Direction;
    case 0x8C376CE7u: //fore_color
        return Id::kForeColor;
    case 0x53B4B849u: //border_size
        return Id::kBorderSize;
    case 0xCE89D74Eu: //bordersize（别名，归一到 border_size）
        return Id::kBorderSize;
    case 0x61307092u: //border_dash_style
        return Id::kBorderDashStyle;
    case 0x64C40180u: //borders_on_top
        return Id::kBordersOnTop;
    case 0x9E59B620u: //border_round
        return Id::kBorderRound;
    case 0x2DFD7D25u: //borderround（别名，归一到 border_round）
        return Id::kBorderRound;
    case 0x4B00B2BFu: //box_shadow
        return Id::kBoxShadow;
    case 0x2C1DD9CAu: //boxshadow（别名，归一到 box_shadow）
        return Id::kBoxShadow;
    case 0x95876E1Fu: //width
        return Id::kWidth;
    case 0x783132F6u: //state
        return Id::kState;
    case 0x4D4150F8u: //cursor_type
        return Id::kCursorType;
    case 0x53CF22F7u: //cursortype（别名，归一到 cursor_type）
        return Id::kCursorType;
    case 0xC543CCB1u: //render_offset
        return Id::kRenderOffset;
    case 0x7703E402u: //renderoffset（别名，归一到 render_offset）
        return Id::kRenderOffset;
    case 0x1E43B3C6u: //normal_color
        return Id::kNormalColor;
    case 0xB9D69A25u: //normalcolor（别名，归一到 normal_color）
        return Id::kNormalColor;
    case 0xD38C7ED4u: //hovered_color
        return Id::kHoveredColor;
    case 0xF68C6B74u: //hot_color
        return Id::kHotColor;
    case 0x29CE06FBu: //hotcolor（别名，归一到 hot_color）
        return Id::kHotColor;
    case 0x33B9CE71u: //pressed_color
        return Id::kPressedColor;
    case 0x07BC1C84u: //pushed_color
        return Id::kPushedColor;
    case 0xA5E4EF0Bu: //pushedcolor（别名，归一到 pushed_color）
        return Id::kPushedColor;
    case 0xF2F34C91u: //disabled_color
        return Id::kDisabledColor;
    case 0x838EB598u: //disabledcolor（别名，归一到 disabled_color）
        return Id::kDisabledColor;
    case 0xB63B4BADu: //normal_color_margin
        return Id::kNormalColorMargin;
    case 0x4A46659Fu: //hovered_color_margin
        return Id::kHoveredColorMargin;
    case 0xBDC95A7Fu: //hot_color_margin
        return Id::kHotColorMargin;
    case 0x3C93B66Cu: //pressed_color_margin
        return Id::kPressedColorMargin;
    case 0x7552166Fu: //pushed_color_margin
        return Id::kPushedColorMargin;
    case 0x8BF68C4Cu: //disabled_color_margin
        return Id::kDisabledColorMargin;
    case 0x9F42179Eu: //state_color_min_width
        return Id::kStateColorMinWidth;
    case 0xC1C87351u: //state_color_min_height
        return Id::kStateColorMinHeight;
    case 0xA57A4D51u: //normal_color_round
        return Id::kNormalColorRound;
    case 0x2CFD809Fu: //hovered_color_round
        return Id::kHoveredColorRound;
    case 0x36CD14FFu: //hot_color_round
        return Id::kHotColorRound;
    case 0xF17CB53Au: //pressed_color_round
        return Id::kPressedColorRound;
    case 0x51B47F6Fu: //pushed_color_round
        return Id::kPushedColorRound;
    case 0x23E2C59Au: //disabled_color_round
        return Id::kDisabledColorRound;
    case 0x94C31ACFu: //border_color
        return Id::kBorderColor;
    case 0x625B5B86u: //bordercolor（别名，归一到 border_color）
        return Id::kBorderColor;
    case 0x608D53CDu: //normal_border_color
        return Id::kNormalBorderColor;
    case 0x8FB91013u: //hovered_border_color
        return Id::kHoveredBorderColor;
    case 0xD2444AF3u: //hot_border_color
        return Id::kHotBorderColor;
    case 0xF8BFD41Cu: //pressed_border_color
        return Id::kPressedBorderColor;
    case 0x3BA2ADC3u: //pushed_border_color
        return Id::kPushedBorderColor;
    case 0xD049D6FCu: //disabled_border_color
        return Id::kDisabledBorderColor;
    case 0xAC3BEBCDu: //focused_border_color
        return Id::kFocusedBorderColor;
    case 0xB4F2CFDEu: //focus_border_color
        return Id::kFocusBorderColor;
    case 0x63FE3091u: //left_border_size
        return Id::kLeftBorderSize;
    case 0x85B0742Fu: //leftbordersize（别名，归一到 left_border_size）
        return Id::kLeftBorderSize;
    case 0xFB195DBDu: //top_border_size
        return Id::kTopBorderSize;
    case 0x2F18DA3Bu: //topbordersize（别名，归一到 top_border_size）
        return Id::kTopBorderSize;
    case 0xB89C1308u: //right_border_size
        return Id::kRightBorderSize;
    case 0xEC65A96Eu: //rightbordersize（别名，归一到 right_border_size）
        return Id::kRightBorderSize;
    case 0x5C84A34Bu: //bottom_border_size
        return Id::kBottomBorderSize;
    case 0xCBBB9C21u: //bottombordersize（别名，归一到 bottom_border_size）
        return Id::kBottomBorderSize;
    case 0x1AA2391Fu: //bkimage
        return Id::kBkimage;
    case 0x8CCB3D54u: //min_width
        return Id::kMinWidth;
    case 0x5514FC59u: //minwidth（别名，归一到 min_width）
        return Id::kMinWidth;
    case 0x40C08F4Eu: //max_width
        return Id::kMaxWidth;
    case 0xB8BFA853u: //maxwidth（别名，归一到 max_width）
        return Id::kMaxWidth;
    case 0x72BD9403u: //min_height
        return Id::kMinHeight;
    case 0x5F92C5C8u: //minheight（别名，归一到 min_height）
        return Id::kMinHeight;
    case 0xB37541C1u: //max_height
        return Id::kMaxHeight;
    case 0x6B9EE08Eu: //maxheight（别名，归一到 max_height）
        return Id::kMaxHeight;
    case 0x8D39BDE6u: //name
        return Id::kName;
    case 0x95446FA6u: //tooltip_text
        return Id::kTooltipText;
    case 0x2F7A1C87u: //tooltiptext（别名，归一到 tooltip_text）
        return Id::kTooltipText;
    case 0xC2097B36u: //tooltip_text_id
        return Id::kTooltipTextId;
    case 0x4BF8493Bu: //tooltip_textid
        return Id::kTooltipTextid;
    case 0x5CBB824Au: //tooltiptextid
        return Id::kTooltiptextid;
    case 0x736A2287u: //tooltip_width
        return Id::kTooltipWidth;
    case 0x89D607D3u: //data_id
        return Id::kDataId;
    case 0xB0CC69C0u: //dataid（别名，归一到 data_id）
        return Id::kDataId;
    case 0x84872F7Du: //user_data_id
        return Id::kUserDataId;
    case 0xF728BF12u: //user_dataid
        return Id::kUserDataid;
    case 0x02F3B39Eu: //enabled
        return Id::kEnabled;
    case 0x189BF66Au: //mouse_enabled
        return Id::kMouseEnabled;
    case 0x592318C4u: //mouse
        return Id::kMouse;
    case 0xB4D00C7Au: //keyboard_enabled
        return Id::kKeyboardEnabled;
    case 0x4AA845F4u: //keyboard
        return Id::kKeyboard;
    case 0x54FA99A1u: //visible
        return Id::kVisible;
    case 0xABFDE986u: //fade_visible
        return Id::kFadeVisible;
    case 0x2836FA57u: //fadevisible（别名，归一到 fade_visible）
        return Id::kFadeVisible;
    case 0xA6C45D85u: //float
        return Id::kFloat;
    case 0x4A523E04u: //keep_float_pos
        return Id::kKeepFloatPos;
    case 0x265DAFE5u: //cache
        return Id::kCache;
    case 0x3E379DE1u: //no_focus
        return Id::kNoFocus;
    case 0x1AEB97EEu: //nofocus（别名，归一到 no_focus）
        return Id::kNoFocus;
    case 0x5D8B6DABu: //alpha
        return Id::kAlpha;
    case 0x392DD06Cu: //normal_image
        return Id::kNormalImage;
    case 0xC57B78B7u: //normalimage（别名，归一到 normal_image）
        return Id::kNormalImage;
    case 0x781CF0A6u: //hovered_image
        return Id::kHoveredImage;
    case 0x9B1E7046u: //hot_image
        return Id::kHotImage;
    case 0xBB4D1F81u: //hotimage（别名，归一到 hot_image）
        return Id::kHotImage;
    case 0x9CFBD723u: //pressed_image
        return Id::kPressedImage;
    case 0xE540DD76u: //pushed_image
        return Id::kPushedImage;
    case 0xDC21FF31u: //pushedimage（别名，归一到 pushed_image）
        return Id::kPushedImage;
    case 0x75820A43u: //disabled_image
        return Id::kDisabledImage;
    case 0xF961893Au: //disabledimage（别名，归一到 disabled_image）
        return Id::kDisabledImage;
    case 0xDCA1B111u: //fore_normal_image
        return Id::kForeNormalImage;
    case 0x1F879725u: //forenormalimage（别名，归一到 fore_normal_image）
        return Id::kForeNormalImage;
    case 0x37AC3DF1u: //fore_hovered_image
        return Id::kForeHoveredImage;
    case 0xE2557D61u: //fore_hot_image
        return Id::kForeHotImage;
    case 0x8F019D53u: //forehotimage（别名，归一到 fore_hot_image）
        return Id::kForeHotImage;
    case 0xFC13C760u: //fore_pressed_image
        return Id::kForePressedImage;
    case 0x5741CA03u: //fore_pushed_image
        return Id::kForePushedImage;
    case 0x0709439Fu: //forepushedimage（别名，归一到 fore_pushed_image）
        return Id::kForePushedImage;
    case 0xF3B954BAu: //fore_disabled_image
        return Id::kForeDisabledImage;
    case 0x88BC9D24u: //foredisabledimage（别名，归一到 fore_disabled_image）
        return Id::kForeDisabledImage;
    case 0x58681118u: //fade_alpha
        return Id::kFadeAlpha;
    case 0x51C2E1D5u: //fadealpha（别名，归一到 fade_alpha）
        return Id::kFadeAlpha;
    case 0x50F89AB7u: //fade_hovered
        return Id::kFadeHovered;
    case 0x6B51AAFFu: //fade_hot
        return Id::kFadeHot;
    case 0xC1A9429Au: //fadehot（别名，归一到 fade_hot）
        return Id::kFadeHot;
    case 0x776CAAD0u: //fade_hovered_frame_interval_ms
        return Id::kFadeHoveredFrameIntervalMs;
    case 0x0282D1B8u: //fade_hot_frame_interval_ms
        return Id::kFadeHotFrameIntervalMs;
    case 0x444CD557u: //fade_hovered_total_ms
        return Id::kFadeHoveredTotalMs;
    case 0x367E88FFu: //fade_hot_total_ms
        return Id::kFadeHotTotalMs;
    case 0xAA66F182u: //fade_hovered_easing_function
        return Id::kFadeHoveredEasingFunction;
    case 0x2531540Au: //fade_hot_easing_function
        return Id::kFadeHotEasingFunction;
    case 0x6748D1A0u: //fade_width
        return Id::kFadeWidth;
    case 0xF0653125u: //fadewidth（别名，归一到 fade_width）
        return Id::kFadeWidth;
    case 0xD6B9A2D7u: //fade_height
        return Id::kFadeHeight;
    case 0x8C3A89FCu: //fadeheight（别名，归一到 fade_height）
        return Id::kFadeHeight;
    case 0xF82C585Du: //fade_size
        return Id::kFadeSize;
    case 0xD56F1938u: //fade_in_out_x_from_left
        return Id::kFadeInOutXFromLeft;
    case 0xAACDA20Du: //fadeinoutxfromleft（别名，归一到 fade_in_out_x_from_left）
        return Id::kFadeInOutXFromLeft;
    case 0x7ABFCD8Du: //fade_in_out_x_from_right
        return Id::kFadeInOutXFromRight;
    case 0x820922C6u: //fadeinoutxfromright（别名，归一到 fade_in_out_x_from_right）
        return Id::kFadeInOutXFromRight;
    case 0xEB7356DDu: //fade_in_out_y_from_top
        return Id::kFadeInOutYFromTop;
    case 0xE065D3F2u: //fadeinoutyfromtop（别名，归一到 fade_in_out_y_from_top）
        return Id::kFadeInOutYFromTop;
    case 0x92CEA119u: //fade_in_out_y_from_bottom
        return Id::kFadeInOutYFromBottom;
    case 0x8FF6AFF4u: //fadeinoutyfrombottom（别名，归一到 fade_in_out_y_from_bottom）
        return Id::kFadeInOutYFromBottom;
    case 0x4347769Cu: //fade_frame_interval_ms
        return Id::kFadeFrameIntervalMs;
    case 0x8AFBA643u: //fade_total_ms
        return Id::kFadeTotalMs;
    case 0x264AF266u: //fade_easing_function
        return Id::kFadeEasingFunction;
    case 0x120C71D9u: //tab_stop
        return Id::kTabStop;
    case 0xF6237084u: //tabstop（别名，归一到 tab_stop）
        return Id::kTabStop;
    case 0x68CC88B7u: //loading
        return Id::kLoading;
    case 0xB27FECDDu: //show_focused_rect
        return Id::kShowFocusedRect;
    case 0x4389EC56u: //show_focus_rect
        return Id::kShowFocusRect;
    case 0x8FA3C385u: //focused_rect_color
        return Id::kFocusedRectColor;
    case 0x28685A06u: //focus_rect_color
        return Id::kFocusRectColor;
    case 0x932FC7BCu: //paint_order
        return Id::kPaintOrder;
    case 0xA9B86684u: //start_image_animation
        return Id::kStartImageAnimation;
    case 0x3FD4B7B1u: //start_gif_play
        return Id::kStartGifPlay;
    case 0x47159382u: //stop_image_animation
        return Id::kStopImageAnimation;
    case 0xF7FC01CBu: //stop_gif_play
        return Id::kStopGifPlay;
    case 0x4CC8E3C6u: //set_image_animation_frame
        return Id::kSetImageAnimationFrame;
    case 0x23E0EF3Fu: //enable_drag_drop
        return Id::kEnableDragDrop;
    case 0x9D8324D1u: //enable_drop_file
        return Id::kEnableDropFile;
    case 0x529B3DB1u: //drop_file_types
        return Id::kDropFileTypes;
    case 0x1CBE4574u: //row_span
        return Id::kRowSpan;
    case 0x34EE203Au: //col_span
        return Id::kColSpan;
    case 0x482BAC04u: //button1_normal_image
        return Id::kButton1NormalImage;
    case 0x8B856D38u: //button1normalimage（别名，归一到 button1_normal_image）
        return Id::kButton1NormalImage;
    case 0x902AFD2Eu: //button1_hovered_image
        return Id::kButton1HoveredImage;
    case 0x43A552AEu: //button1_hot_image
        return Id::kButton1HotImage;
    case 0x681EC1E8u: //button1hotimage（别名，归一到 button1_hot_image）
        return Id::kButton1HotImage;
    case 0x812E472Bu: //button1_pressed_image
        return Id::kButton1PressedImage;
    case 0xF271FF9Eu: //button1_pushed_image
        return Id::kButton1PushedImage;
    case 0xFF4951AAu: //button1pushedimage（别名，归一到 button1_pushed_image）
        return Id::kButton1PushedImage;
    case 0xFB2FE4BBu: //button1_disabled_image
        return Id::kButton1DisabledImage;
    case 0xD618CBB5u: //button1disabledimage（别名，归一到 button1_disabled_image）
        return Id::kButton1DisabledImage;
    case 0x0C9AA8AFu: //button2_normal_image
        return Id::kButton2NormalImage;
    case 0x21488F4Bu: //button2normalimage（别名，归一到 button2_normal_image）
        return Id::kButton2NormalImage;
    case 0x923AAF63u: //button2_hovered_image
        return Id::kButton2HoveredImage;
    case 0x4BB52743u: //button2_hot_image
        return Id::kButton2HotImage;
    case 0x2C809D0Du: //button2hotimage（别名，归一到 button2_hot_image）
        return Id::kButton2HotImage;
    case 0x8150F28Au: //button2_pressed_image
        return Id::kButton2PressedImage;
    case 0x4B1EE9E1u: //button2_pushed_image
        return Id::kButton2PushedImage;
    case 0x2E92AECDu: //button2pushedimage（别名，归一到 button2_pushed_image）
        return Id::kButton2PushedImage;
    case 0xF7160F34u: //button2_disabled_image
        return Id::kButton2DisabledImage;
    case 0xF006A83Eu: //button2disabledimage（别名，归一到 button2_disabled_image）
        return Id::kButton2DisabledImage;
    case 0x99AAB32Fu: //rail_normal_image
        return Id::kRailNormalImage;
    case 0x944DF5CBu: //railnormalimage（别名，归一到 rail_normal_image）
        return Id::kRailNormalImage;
    case 0x227B36E3u: //rail_hovered_image
        return Id::kRailHoveredImage;
    case 0x553436C3u: //rail_hot_image
        return Id::kRailHotImage;
    case 0x0A2A408Du: //railhotimage（别名，归一到 rail_hot_image）
        return Id::kRailHotImage;
    case 0x11917A0Au: //rail_pressed_image
        return Id::kRailPressedImage;
    case 0xD82EF461u: //rail_pushed_image
        return Id::kRailPushedImage;
    case 0xA198154Du: //railpushedimage（别名，归一到 rail_pushed_image）
        return Id::kRailPushedImage;
    case 0x8CAB5DB4u: //rail_disabled_image
        return Id::kRailDisabledImage;
    case 0x7D16B2BEu: //raildisabledimage（别名，归一到 rail_disabled_image）
        return Id::kRailDisabledImage;
    case 0x554BFC7Au: //bk_normal_image
        return Id::kBkNormalImage;
    case 0x3FA0187Eu: //bknormalimage（别名，归一到 bk_normal_image）
        return Id::kBkNormalImage;
    case 0x85CFC718u: //bk_hovered_image
        return Id::kBkHoveredImage;
    case 0xF67D0F88u: //bk_hot_image
        return Id::kBkHotImage;
    case 0x709AD532u: //bkhotimage（别名，归一到 bk_hot_image）
        return Id::kBkHotImage;
    case 0xB1C53495u: //bk_pressed_image
        return Id::kBkPressedImage;
    case 0x2A87ABF4u: //bk_pushed_image
        return Id::kBkPushedImage;
    case 0x1B2A6A50u: //bkpushedimage（别名，归一到 bk_pushed_image）
        return Id::kBkPushedImage;
    case 0x80CD8E1Du: //bk_disabled_image
        return Id::kBkDisabledImage;
    case 0x51C59FC7u: //bkdisabledimage（别名，归一到 bk_disabled_image）
        return Id::kBkDisabledImage;
    case 0xB200BCB9u: //line_size
        return Id::kLineSize;
    case 0x79C19C3Eu: //linesize（别名，归一到 line_size）
        return Id::kLineSize;
    case 0xB3681E65u: //thumb_min_length
        return Id::kThumbMinLength;
    case 0x83547685u: //thumbminlength（别名，归一到 thumb_min_length）
        return Id::kThumbMinLength;
    case 0xFADC0CD2u: //range
        return Id::kRange;
    case 0xD1ACD534u: //show_button1
        return Id::kShowButton1;
    case 0x14B64B8Bu: //showbutton1（别名，归一到 show_button1）
        return Id::kShowButton1;
    case 0xD4ACD9EDu: //show_button2
        return Id::kShowButton2;
    case 0x15B64D1Eu: //showbutton2（别名，归一到 show_button2）
        return Id::kShowButton2;
    case 0xED039CA5u: //auto_hide_scroll
        return Id::kAutoHideScroll;
    case 0x5C6C1307u: //autohidescroll（别名，归一到 auto_hide_scroll）
        return Id::kAutoHideScroll;
    case 0x4E0A1774u: //selected
        return Id::kSelected;
    case 0x5FF6ED72u: //switch_select
        return Id::kSwitchSelect;
    case 0x65EC7389u: //switchselect（别名，归一到 switch_select）
        return Id::kSwitchSelect;
    case 0x34650B2Bu: //support_check_Mode
        return Id::kSupportCheckMode;
    case 0xA13BF074u: //auto_select_check
        return Id::kAutoSelectCheck;
    case 0x9C1ADF73u: //normal_first
        return Id::kNormalFirst;
    case 0xD062F558u: //normalfirst（别名，归一到 normal_first）
        return Id::kNormalFirst;
    case 0x197DBC48u: //selected_normal_image
        return Id::kSelectedNormalImage;
    case 0x1577158Cu: //selectednormalimage（别名，归一到 selected_normal_image）
        return Id::kSelectedNormalImage;
    case 0x3836A252u: //selected_hovered_image
        return Id::kSelectedHoveredImage;
    case 0xB1584B5Au: //selected_hot_image
        return Id::kSelectedHotImage;
    case 0xA273F014u: //selectedhotimage（别名，归一到 selected_hot_image）
        return Id::kSelectedHotImage;
    case 0x24DBF49Fu: //selected_pressed_image
        return Id::kSelectedPressedImage;
    case 0xD0F9AA7Au: //selected_pushed_image
        return Id::kSelectedPushedImage;
    case 0xB00E3886u: //selectedpushedimage（别名，归一到 selected_pushed_image）
        return Id::kSelectedPushedImage;
    case 0x8BEC555Fu: //selected_disabled_image
        return Id::kSelectedDisabledImage;
    case 0xB8EB4479u: //selecteddisabledimage（别名，归一到 selected_disabled_image）
        return Id::kSelectedDisabledImage;
    case 0xE7541C05u: //selected_fore_normal_image
        return Id::kSelectedForeNormalImage;
    case 0x9F58F2FEu: //selectedforenormalimage（别名，归一到 selected_fore_normal_image）
        return Id::kSelectedForeNormalImage;
    case 0x128F0FF5u: //selected_fore_hovered_image
        return Id::kSelectedForeHoveredImage;
    case 0x0D44DBADu: //selected_fore_hot_image
        return Id::kSelectedForeHotImage;
    case 0x4BC254B2u: //selectedforehotimage（别名，归一到 selected_fore_hot_image）
        return Id::kSelectedForeHotImage;
    case 0x4D30CFF4u: //selected_fore_pressed_image
        return Id::kSelectedForePressedImage;
    case 0xB93A065Fu: //selected_fore_pushed_image
        return Id::kSelectedForePushedImage;
    case 0x7AE344D0u: //selectedforepushedimage（别名，归一到 selected_fore_pushed_image）
        return Id::kSelectedForePushedImage;
    case 0xC9BE0EFEu: //selected_fore_disabled_image
        return Id::kSelectedForeDisabledImage;
    case 0x61B83E47u: //selectedforedisabledimage（别名，归一到 selected_fore_disabled_image）
        return Id::kSelectedForeDisabledImage;
    case 0x75970784u: //part_selected_normal_image
        return Id::kPartSelectedNormalImage;
    case 0x902C07AEu: //part_selected_hovered_image
        return Id::kPartSelectedHoveredImage;
    case 0x6B92B52Eu: //part_selected_hot_image
        return Id::kPartSelectedHotImage;
    case 0x812F51ABu: //part_selected_pressed_image
        return Id::kPartSelectedPressedImage;
    case 0x1FDD5B1Eu: //part_selected_pushed_image
        return Id::kPartSelectedPushedImage;
    case 0x7CD36C3Bu: //part_selected_disabled_image
        return Id::kPartSelectedDisabledImage;
    case 0xC2CC8D79u: //part_selected_fore_normal_image
        return Id::kPartSelectedForeNormalImage;
    case 0x0F97F1E9u: //part_selected_fore_hovered_image
        return Id::kPartSelectedForeHoveredImage;
    case 0x67F88EF9u: //part_selected_fore_hot_image
        return Id::kPartSelectedForeHotImage;
    case 0x53017458u: //part_selected_fore_pressed_image
        return Id::kPartSelectedForePressedImage;
    case 0x238552FBu: //part_selected_fore_pushed_image
        return Id::kPartSelectedForePushedImage;
    case 0xB958DD62u: //part_selected_fore_disabled_image
        return Id::kPartSelectedForeDisabledImage;
    case 0x06FD7CB6u: //selected_text_color
        return Id::kSelectedTextColor;
    case 0x4408E6C2u: //selectedtextcolor（别名，归一到 selected_text_color）
        return Id::kSelectedTextColor;
    case 0x47913DD4u: //selected_normal_text_color
        return Id::kSelectedNormalTextColor;
    case 0x121A70C7u: //selectednormaltextcolor（别名，归一到 selected_normal_text_color）
        return Id::kSelectedNormalTextColor;
    case 0x4F10068Au: //selected_hovered_text_color
        return Id::kSelectedHoveredTextColor;
    case 0x9AAC14B2u: //selected_hot_text_color
        return Id::kSelectedHotTextColor;
    case 0x9850570Fu: //selectedhottextcolor（别名，归一到 selected_hot_text_color）
        return Id::kSelectedHotTextColor;
    case 0x4427E7EDu: //selected_pressed_text_color
        return Id::kSelectedPressedTextColor;
    case 0xF66C5412u: //selected_pushed_text_color
        return Id::kSelectedPushedTextColor;
    case 0x7AE82875u: //selectedpushedtextcolor（别名，归一到 selected_pushed_text_color）
        return Id::kSelectedPushedTextColor;
    case 0xEC43FBADu: //selected_disabled_text_color
        return Id::kSelectedDisabledTextColor;
    case 0x8CE8D2FAu: //selecteddisabledtextcolor（别名，归一到 selected_disabled_text_color）
        return Id::kSelectedDisabledTextColor;
    case 0xED7E6712u: //selected_normal_color
        return Id::kSelectedNormalColor;
    case 0x2451A0E6u: //selectednormalcolor（别名，归一到 selected_normal_color）
        return Id::kSelectedNormalColor;
    case 0x76E02AA0u: //selected_hovered_color
        return Id::kSelectedHoveredColor;
    case 0x3B8577B8u: //selected_hot_color
        return Id::kSelectedHotColor;
    case 0x94D612DEu: //selectedhotcolor（别名，归一到 selected_hot_color）
        return Id::kSelectedHotColor;
    case 0x645D461Du: //selected_pressed_color
        return Id::kSelectedPressedColor;
    case 0x5B26D6D8u: //selected_pushed_color
        return Id::kSelectedPushedColor;
    case 0x0B7DC6B4u: //selectedpushedcolor（别名，归一到 selected_pushed_color）
        return Id::kSelectedPushedColor;
    case 0xE73657DDu: //selected_disabled_color
        return Id::kSelectedDisabledColor;
    case 0x4B40B403u: //selecteddisabledcolor（别名，归一到 selected_disabled_color）
        return Id::kSelectedDisabledColor;
    case 0x33C7D0F1u: //selected_normal_color_margin
        return Id::kSelectedNormalColorMargin;
    case 0x10CAD193u: //selected_hovered_color_margin
        return Id::kSelectedHoveredColorMargin;
    case 0xC21C865Bu: //selected_hot_color_margin
        return Id::kSelectedHotColorMargin;
    case 0xCC754300u: //selected_pressed_color_margin
        return Id::kSelectedPressedColorMargin;
    case 0xF321523Bu: //selected_pushed_color_margin
        return Id::kSelectedPushedColorMargin;
    case 0x30D425C0u: //selected_disabled_color_margin
        return Id::kSelectedDisabledColorMargin;
    case 0xC8C1C125u: //selected_normal_color_round
        return Id::kSelectedNormalColorRound;
    case 0x0EEDDB53u: //selected_hovered_color_round
        return Id::kSelectedHoveredColorRound;
    case 0x386ABD9Bu: //selected_hot_color_round
        return Id::kSelectedHotColorRound;
    case 0xFB4B92DEu: //selected_pressed_color_round
        return Id::kSelectedPressedColorRound;
    case 0xC67F44FBu: //selected_pushed_color_round
        return Id::kSelectedPushedColorRound;
    case 0xC6312B9Eu: //selected_disabled_color_round
        return Id::kSelectedDisabledColorRound;
    case 0xFECE98D4u: //corner_size
        return Id::kCornerSize;
    case 0x328F4C1Eu: //url
        return Id::kUrl;
    case 0x678294AEu: //show_url_tooltip
        return Id::kShowUrlTooltip;
    case 0x5FB91E8Cu: //group
        return Id::kGroup;
    case 0x95E8960Au: //enable_split_single
        return Id::kEnableSplitSingle;
    case 0x3E4E52C8u: //drag_alpha
        return Id::kDragAlpha;
    case 0xA6A75BD8u: //drag_out
        return Id::kDragOut;
    case 0xA5C388EFu: //enable_move_pos
        return Id::kEnableMovePos;
    case 0x687D0B20u: //move_parent_pos
        return Id::kMoveParentPos;
    case 0xBF0F21D6u: //move_pos_alpha
        return Id::kMovePosAlpha;
    case 0xCC19A21Cu: //move_pos_non_draggable_margin
        return Id::kMovePosNonDraggableMargin;
    case 0x8437A4C4u: //move_pos_draggable_border
        return Id::kMovePosDraggableBorder;
    case 0x6963D7F9u: //move_pos_reserve_width
        return Id::kMovePosReserveWidth;
    case 0x106FEDE8u: //move_pos_reserve_height
        return Id::kMovePosReserveHeight;
    case 0x2EDB038Cu: //move_pos_keep_within_parent
        return Id::kMovePosKeepWithinParent;
    case 0x8A19FC3Bu: //enable_resize
        return Id::kEnableResize;
    case 0xC504BD7Du: //resize_size_box
        return Id::kResizeSizeBox;
    case 0xA0F0D1BDu: //resize_reserve_width
        return Id::kResizeReserveWidth;
    case 0x7A721914u: //resize_reserve_height
        return Id::kResizeReserveHeight;
    case 0x1B3F55E8u: //resize_keep_within_parent
        return Id::kResizeKeepWithinParent;
    case 0xE77BCA42u: //url_is_local_file
        return Id::kUrlIsLocalFile;
    case 0xDEBD6C9Eu: //F12
        return Id::kF12;
    case 0xDDBD6B0Bu: //F11
        return Id::kF11;
    case 0xBF557F3Cu: //download_favicon_image
        return Id::kDownloadFaviconImage;
    case 0xFBE6E7F1u: //devtools_enabled
        return Id::kDevtoolsEnabled;
    case 0x2A37B398u: //rows
        return Id::kRows;
    case 0x8AB31A3Cu: //columns
        return Id::kColumns;
    case 0xD74E9376u: //grid_width
        return Id::kGridWidth;
    case 0xF656DC89u: //grid_height
        return Id::kGridHeight;
    case 0x96E1F7C2u: //scale_down
        return Id::kScaleDown;
    case 0xF7148201u: //scaledown（别名，归一到 scale_down）
        return Id::kScaleDown;
    case 0x1677065Au: //item_size
        return Id::kItemSize;
    case 0x1F455FE7u: //itemsize（别名，归一到 item_size）
        return Id::kItemSize;
    case 0x54605FE4u: //auto_calc_item_size
        return Id::kAutoCalcItemSize;
    case 0x3919BB40u: //child_margin
        return Id::kChildMargin;
    case 0x49425B63u: //childmargin（别名，归一到 child_margin）
        return Id::kChildMargin;
    case 0xA6F85FEFu: //child_margin_x
        return Id::kChildMarginX;
    case 0x6E756B81u: //childmarginx（别名，归一到 child_margin_x）
        return Id::kChildMarginX;
    case 0xA5F85E5Cu: //child_margin_y
        return Id::kChildMarginY;
    case 0x6D7569EEu: //childmarginy（别名，归一到 child_margin_y）
        return Id::kChildMarginY;
    case 0xDD2CFBBDu: //child_valign
        return Id::kChildValign;
    case 0x95B39AFBu: //child_halign
        return Id::kChildHalign;
    case 0x530F2027u: //child_align
        return Id::kChildAlign;
    default:
        break;
    }
    return Id::kInvalidId;
}

DString IdToString(Id id)
{
    switch (id) {
    case Id::kMultiSelect:
        return _T("multi_select");
    case Id::kPaintSelectedColors:
        return _T("paint_selected_colors");
    case Id::kScrollSelect:
        return _T("scroll_select");
    case Id::kSelectNextWhenActiveRemoved:
        return _T("select_next_when_active_removed");
    case Id::kFrameSelection:
        return _T("frame_selection");
    case Id::kFrameSelectionColor:
        return _T("frame_selection_color");
    case Id::kFrameSelectionAlpha:
        return _T("frame_selection_alpha");
    case Id::kFrameSelectionBorderSize:
        return _T("frame_selection_border_size");
    case Id::kFrameSelectionBorderColor:
        return _T("frame_selection_border_color");
    case Id::kSelectNoneWhenClickBlank:
        return _T("select_none_when_click_blank");
    case Id::kSelectLikeListCtrl:
        return _T("select_like_list_ctrl");
    case Id::kVscrollbar:
        return _T("vscrollbar");
    case Id::kVscrollbarStyle:
        return _T("vscrollbar_style");
    case Id::kVscrollbarClass:
        return _T("vscrollbar_class");
    case Id::kHscrollbar:
        return _T("hscrollbar");
    case Id::kHscrollbarStyle:
        return _T("hscrollbar_style");
    case Id::kHscrollbarClass:
        return _T("hscrollbar_class");
    case Id::kScrollbarPadding:
        return _T("scrollbar_padding");
    case Id::kVscrollUnit:
        return _T("vscroll_unit");
    case Id::kHscrollUnit:
        return _T("hscroll_unit");
    case Id::kScrollbarFloat:
        return _T("scrollbar_float");
    case Id::kVscrollbarLeft:
        return _T("vscrollbar_left");
    case Id::kHoldEnd:
        return _T("hold_end");
    case Id::kSelectedId:
        return _T("selected_id");
    case Id::kFadeSwitch:
        return _T("fade_switch");
    case Id::kFadeSwitchType:
        return _T("fade_switch_type");
    case Id::kFadeSwitchFrameIntervalMs:
        return _T("fade_switch_frame_interval_ms");
    case Id::kFadeSwitchTotalMs:
        return _T("fade_switch_total_ms");
    case Id::kFadeSwitchEasingFunction:
        return _T("fade_switch_easing_function");
    case Id::kXmlFilePath:
        return _T("xml_file_path");
    case Id::kResPath:
        return _T("res_path");
    case Id::kAddressPath:
        return _T("address_path");
    case Id::kPathTooltip:
        return _T("path_tooltip");
    case Id::kReturnUpdateUi:
        return _T("return_update_ui");
    case Id::kEscUpdateUi:
        return _T("esc_update_ui");
    case Id::kKillFocusUpdateUi:
        return _T("kill_focus_update_ui");
    case Id::kRichEditClass:
        return _T("rich_edit_class");
    case Id::kRichEditClearBtnClass:
        return _T("rich_edit_clear_btn_class");
    case Id::kSubPathHboxClass:
        return _T("sub_path_hbox_class");
    case Id::kSubPathButtonClass:
        return _T("sub_path_button_class");
    case Id::kSubPathRootClass:
        return _T("sub_path_root_class");
    case Id::kPathSeparatorClass:
        return _T("path_separator_class");
    case Id::kBitmapHalign:
        return _T("bitmap_halign");
    case Id::kBitmapValign:
        return _T("bitmap_valign");
    case Id::kBitmapAlpha:
        return _T("bitmap_alpha");
    case Id::kBitmapDest:
        return _T("bitmap_dest");
    case Id::kBitmapSrc:
        return _T("bitmap_src");
    case Id::kBitmapMargin:
        return _T("bitmap_margin");
    case Id::kBitmapAdaptiveDestRect:
        return _T("bitmap_adaptive_dest_rect");
    case Id::kBitmapStretch:
        return _T("bitmap_stretch");
    case Id::kBitmapMultiThread:
        return _T("bitmap_multi_thread");
    case Id::kBitmapFile:
        return _T("bitmap_file");
    case Id::kDropbox:
        return _T("dropbox");
    case Id::kDropboxItemClass:
        return _T("dropbox_item_class");
    case Id::kSelectedItemClass:
        return _T("selected_item_class");
    case Id::kDropboxSize:
        return _T("dropbox_size");
    case Id::kPopupTop:
        return _T("popup_top");
    case Id::kHeight:
        return _T("height");
    case Id::kShadowType:
        return _T("shadow_type");
    case Id::kChildWindowMargin:
        return _T("child_window_margin");
    case Id::kCircular:
        return _T("circular");
    case Id::kCircleWidth:
        return _T("circle_width");
    case Id::kIndicator:
        return _T("indicator");
    case Id::kClockwise:
        return _T("clockwise");
    case Id::kBgcolor:
        return _T("bgcolor");
    case Id::kFgcolor:
        return _T("fgcolor");
    case Id::kGradientColor:
        return _T("gradient_color");
    case Id::kCursorFile:
        return _T("cursor_file");
    case Id::kColorType:
        return _T("color_type");
    case Id::kComboType:
        return _T("combo_type");
    case Id::kComboTreeViewClass:
        return _T("combo_tree_view_class");
    case Id::kComboTreeNodeClass:
        return _T("combo_tree_node_class");
    case Id::kComboIconClass:
        return _T("combo_icon_class");
    case Id::kComboEditClass:
        return _T("combo_edit_class");
    case Id::kComboButtonClass:
        return _T("combo_button_class");
    case Id::kComboBoxClass:
        return _T("combo_box_class");
    case Id::kLeftButtonClass:
        return _T("left_button_class");
    case Id::kLeftButtonTopLabelClass:
        return _T("left_button_top_label_class");
    case Id::kLeftButtonBottomLabelClass:
        return _T("left_button_bottom_label_class");
    case Id::kLeftButtonTopLabelText:
        return _T("left_button_top_label_text");
    case Id::kLeftButtonBottomLabelText:
        return _T("left_button_bottom_label_text");
    case Id::kLeftButtonTopLabelBkcolor:
        return _T("left_button_top_label_bkcolor");
    case Id::kLeftButtonBottomLabelBkcolor:
        return _T("left_button_bottom_label_bkcolor");
    case Id::kRightButtonClass:
        return _T("right_button_class");
    case Id::kFormat:
        return _T("format");
    case Id::kEditFormat:
        return _T("edit_format");
    case Id::kSpinClass:
        return _T("spin_class");
    case Id::kSmallIconSize:
        return _T("small_icon_size");
    case Id::kLargeIconSize:
        return _T("large_icon_size");
    case Id::kShowHidenFiles:
        return _T("show_hiden_files");
    case Id::kShowSystemFiles:
        return _T("show_system_files");
    case Id::kDefaultText:
        return _T("default_text");
    case Id::kDefaultTextId:
        return _T("default_text_id");
    case Id::kIp:
        return _T("ip");
    case Id::kTextAlign:
        return _T("text_align");
    case Id::kEndEllipsis:
        return _T("end_ellipsis");
    case Id::kPathEllipsis:
        return _T("path_ellipsis");
    case Id::kSingleLine:
        return _T("single_line");
    case Id::kMultiLine:
        return _T("multi_line");
    case Id::kText:
        return _T("text");
    case Id::kTextId:
        return _T("text_id");
    case Id::kAutoTooltip:
        return _T("auto_tooltip");
    case Id::kFont:
        return _T("font");
    case Id::kTextColor:
        return _T("text_color");
    case Id::kNormalTextColor:
        return _T("normal_text_color");
    case Id::kHoveredTextColor:
        return _T("hovered_text_color");
    case Id::kHotTextColor:
        return _T("hot_text_color");
    case Id::kPressedTextColor:
        return _T("pressed_text_color");
    case Id::kPushedTextColor:
        return _T("pushed_text_color");
    case Id::kDisabledTextColor:
        return _T("disabled_text_color");
    case Id::kTextPadding:
        return _T("text_padding");
    case Id::kReplaceNewline:
        return _T("replace_newline");
    case Id::kSpacingMul:
        return _T("spacing_mul");
    case Id::kSpacingAdd:
        return _T("spacing_add");
    case Id::kVerticalText:
        return _T("vertical_text");
    case Id::kWordSpacing:
        return _T("word_spacing");
    case Id::kUseFontHeight:
        return _T("use_font_height");
    case Id::kAsciiRotate90:
        return _T("ascii_rotate_90");
    case Id::kRichText:
        return _T("rich_text");
    case Id::kVertical:
        return _T("vertical");
    case Id::kLineColor:
        return _T("line_color");
    case Id::kLineWidth:
        return _T("line_width");
    case Id::kDashStyle:
        return _T("dash_style");
    case Id::kHeaderClass:
        return _T("header_class");
    case Id::kHeaderItemClass:
        return _T("header_item_class");
    case Id::kHeaderSplitBoxClass:
        return _T("header_split_box_class");
    case Id::kHeaderSplitControlClass:
        return _T("header_split_control_class");
    case Id::kEnableHeaderDragOrder:
        return _T("enable_header_drag_order");
    case Id::kCheckBoxClass:
        return _T("check_box_class");
    case Id::kDataItemClass:
        return _T("data_item_class");
    case Id::kDataSubItemClass:
        return _T("data_sub_item_class");
    case Id::kRowGridLineWidth:
        return _T("row_grid_line_width");
    case Id::kRowGridLineColor:
        return _T("row_grid_line_color");
    case Id::kColumnGridLineWidth:
        return _T("column_grid_line_width");
    case Id::kColumnGridLineColor:
        return _T("column_grid_line_color");
    case Id::kReportViewClass:
        return _T("report_view_class");
    case Id::kHeaderHeight:
        return _T("header_height");
    case Id::kDataItemHeight:
        return _T("data_item_height");
    case Id::kShowHeader:
        return _T("show_header");
    case Id::kEnableColumnWidthAuto:
        return _T("enable_column_width_auto");
    case Id::kAutoCheckSelect:
        return _T("auto_check_select");
    case Id::kShowHeaderCheckbox:
        return _T("show_header_checkbox");
    case Id::kShowDataItemCheckbox:
        return _T("show_data_item_checkbox");
    case Id::kType:
        return _T("type");
    case Id::kIconViewClass:
        return _T("icon_view_class");
    case Id::kIconViewItemClass:
        return _T("icon_view_item_class");
    case Id::kIconViewItemImageClass:
        return _T("icon_view_item_image_class");
    case Id::kIconViewItemLabelClass:
        return _T("icon_view_item_label_class");
    case Id::kListViewClass:
        return _T("list_view_class");
    case Id::kListViewItemClass:
        return _T("list_view_item_class");
    case Id::kListViewItemImageClass:
        return _T("list_view_item_image_class");
    case Id::kListViewItemLabelClass:
        return _T("list_view_item_label_class");
    case Id::kEnableItemEdit:
        return _T("enable_item_edit");
    case Id::kListCtrlRicheditClass:
        return _T("list_ctrl_richedit_class");
    case Id::kIconSpacing:
        return _T("icon_spacing");
    case Id::kSortedUpImage:
        return _T("sorted_up_image");
    case Id::kSortedDownImage:
        return _T("sorted_down_image");
    case Id::kShowIconAtTop:
        return _T("show_icon_at_top");
    case Id::kHorizontalLayout:
        return _T("horizontal_layout");
    case Id::kHorizontal:
        return _T("horizontal");
    case Id::kHor:
        return _T("hor");
    case Id::kMin:
        return _T("min");
    case Id::kMax:
        return _T("max");
    case Id::kValue:
        return _T("value");
    case Id::kProgressImage:
        return _T("progress_image");
    case Id::kStretchForeImage:
        return _T("stretch_fore_image");
    case Id::kIsStretchFore:
        return _T("is_stretch_fore");
    case Id::kProgressColor:
        return _T("progress_color");
    case Id::kMarquee:
        return _T("marquee");
    case Id::kMarqueeWidth:
        return _T("marquee_width");
    case Id::kMarqueeStep:
        return _T("marquee_step");
    case Id::kReverse:
        return _T("reverse");
    case Id::kPropertyGridXml:
        return _T("property_grid_xml");
    case Id::kGroupClass:
        return _T("group_class");
    case Id::kGroupLabelClass:
        return _T("group_label_class");
    case Id::kPropertyClass:
        return _T("property_class");
    case Id::kPropertyNameLabelClass:
        return _T("property_name_label_class");
    case Id::kPropertyValueLabelClass:
        return _T("property_value_label_class");
    case Id::kLeftColumnWidth:
        return _T("left_column_width");
    case Id::kPropertyFontNormal:
        return _T("property_font_normal");
    case Id::kPropertyFontModified:
        return _T("property_font_modified");
    case Id::kReadonly:
        return _T("readonly");
    case Id::kPassword:
        return _T("password");
    case Id::kShowPassword:
        return _T("show_password");
    case Id::kPasswordChar:
        return _T("password_char");
    case Id::kFlashPasswordChar:
        return _T("flash_password_char");
    case Id::kNumberOnly:
        return _T("number_only");
    case Id::kNumber:
        return _T("number");
    case Id::kMaxNumber:
        return _T("max_number");
    case Id::kMinNumber:
        return _T("min_number");
    case Id::kNumberFormat:
        return _T("number_format");
    case Id::kCaretColor:
        return _T("caret_color");
    case Id::kPromptMode:
        return _T("prompt_mode");
    case Id::kPromptColor:
        return _T("prompt_color");
    case Id::kPromptText:
        return _T("prompt_text");
    case Id::kPromptTextId:
        return _T("prompt_text_id");
    case Id::kPromptTextid:
        return _T("prompt_textid");
    case Id::kPrompttextid:
        return _T("prompttextid");
    case Id::kFocusedImage:
        return _T("focused_image");
    case Id::kWantTab:
        return _T("want_tab");
    case Id::kWantReturn:
        return _T("want_return");
    case Id::kWantReturnMsg:
        return _T("want_return_msg");
    case Id::kWantCtrlReturn:
        return _T("want_ctrl_return");
    case Id::kReturnMsgWantCtrl:
        return _T("return_msg_want_ctrl");
    case Id::kLimitText:
        return _T("limit_text");
    case Id::kMaxChar:
        return _T("max_char");
    case Id::kLimitChars:
        return _T("limit_chars");
    case Id::kWordWrap:
        return _T("word_wrap");
    case Id::kNoCaretReadonly:
        return _T("no_caret_readonly");
    case Id::kDefaultContextMenu:
        return _T("default_context_menu");
    case Id::kClearBtnClass:
        return _T("clear_btn_class");
    case Id::kShowPasswordBtnClass:
        return _T("show_password_btn_class");
    case Id::kWheelZoom:
        return _T("wheel_zoom");
    case Id::kHideSelection:
        return _T("hide_selection");
    case Id::kFocusedBottomBorderSize:
        return _T("focused_bottom_border_size");
    case Id::kFocusBottomBorderSize:
        return _T("focus_bottom_border_size");
    case Id::kFocusedBottomBorderColor:
        return _T("focused_bottom_border_color");
    case Id::kFocusBottomBorderColor:
        return _T("focus_bottom_border_color");
    case Id::kZoom:
        return _T("zoom");
    case Id::kAutoVscroll:
        return _T("auto_vscroll");
    case Id::kAutoHscroll:
        return _T("auto_hscroll");
    case Id::kRich:
        return _T("rich");
    case Id::kAutoDetectUrl:
        return _T("auto_detect_url");
    case Id::kAllowBeep:
        return _T("allow_beep");
    case Id::kSaveSelection:
        return _T("save_selection");
    case Id::kSelectAllOnFocused:
        return _T("select_all_on_focused");
    case Id::kSelectAllOnFocus:
        return _T("select_all_on_focus");
    case Id::kSelectionBkcolor:
        return _T("selection_bkcolor");
    case Id::kInactiveSelectionBkcolor:
        return _T("inactive_selection_bkcolor");
    case Id::kCurrentRowBkcolor:
        return _T("current_row_bkcolor");
    case Id::kInactiveCurrentRowBkcolor:
        return _T("inactive_current_row_bkcolor");
    case Id::kRowSpacingMul:
        return _T("row_spacing_mul");
    case Id::kRowSpacingAdd:
        return _T("row_spacing_add");
    case Id::kEnableDragOut:
        return _T("enable_drag_out");
    case Id::kDefaultLinkFontColor:
        return _T("default_link_font_color");
    case Id::kHoveredLinkFontColor:
        return _T("hovered_link_font_color");
    case Id::kHoverLinkFontColor:
        return _T("hover_link_font_color");
    case Id::kPressedLinkFontColor:
        return _T("pressed_link_font_color");
    case Id::kMouseDownLinkFontColor:
        return _T("mouse_down_link_font_color");
    case Id::kLinkFontUnderline:
        return _T("link_font_underline");
    case Id::kReplaceBrace:
        return _T("replace_brace");
    case Id::kTrimPolicy:
        return _T("trim_policy");
    case Id::kStep:
        return _T("step");
    case Id::kThumbNormalImage:
        return _T("thumb_normal_image");
    case Id::kThumbHoveredImage:
        return _T("thumb_hovered_image");
    case Id::kThumbHotImage:
        return _T("thumb_hot_image");
    case Id::kThumbPressedImage:
        return _T("thumb_pressed_image");
    case Id::kThumbPushedImage:
        return _T("thumb_pushed_image");
    case Id::kThumbDisabledImage:
        return _T("thumb_disabled_image");
    case Id::kThumbSize:
        return _T("thumb_size");
    case Id::kProgressBarPadding:
        return _T("progress_bar_padding");
    case Id::kTabBoxName:
        return _T("tab_box_name");
    case Id::kDragOrder:
        return _T("drag_order");
    case Id::kSelectedTabItemOutlineWidth:
        return _T("selected_tab_item_outline_width");
    case Id::kSelectedTabItemOutlineColor:
        return _T("selected_tab_item_outline_color");
    case Id::kTabCtrlBottomLineHeight:
        return _T("tab_ctrl_bottom_line_height");
    case Id::kTabCtrlBottomLineColor:
        return _T("tab_ctrl_bottom_line_color");
    case Id::kTabBoxItemIndex:
        return _T("tab_box_item_index");
    case Id::kTitle:
        return _T("title");
    case Id::kTitleId:
        return _T("title_id");
    case Id::kIcon:
        return _T("icon");
    case Id::kIconClass:
        return _T("icon_class");
    case Id::kTitleClass:
        return _T("title_class");
    case Id::kCloseButtonClass:
        return _T("close_button_class");
    case Id::kLineClass:
        return _T("line_class");
    case Id::kSelectedRoundCorner:
        return _T("selected_round_corner");
    case Id::kHoveredRoundCorner:
        return _T("hovered_round_corner");
    case Id::kHotRoundCorner:
        return _T("hot_round_corner");
    case Id::kHoveredPadding:
        return _T("hovered_padding");
    case Id::kHotPadding:
        return _T("hot_padding");
    case Id::kAutoHideCloseButton:
        return _T("auto_hide_close_button");
    case Id::kExpandNormalImage:
        return _T("expand_normal_image");
    case Id::kExpandHoveredImage:
        return _T("expand_hovered_image");
    case Id::kExpandHotImage:
        return _T("expand_hot_image");
    case Id::kExpandPressedImage:
        return _T("expand_pressed_image");
    case Id::kExpandPushedImage:
        return _T("expand_pushed_image");
    case Id::kExpandDisabledImage:
        return _T("expand_disabled_image");
    case Id::kCollapseNormalImage:
        return _T("collapse_normal_image");
    case Id::kCollapseHoveredImage:
        return _T("collapse_hovered_image");
    case Id::kCollapseHotImage:
        return _T("collapse_hot_image");
    case Id::kCollapsePressedImage:
        return _T("collapse_pressed_image");
    case Id::kCollapsePushedImage:
        return _T("collapse_pushed_image");
    case Id::kCollapseDisabledImage:
        return _T("collapse_disabled_image");
    case Id::kExpandImageRightSpace:
        return _T("expand_image_right_space");
    case Id::kCheckBoxImageRightSpace:
        return _T("check_box_image_right_space");
    case Id::kIconImageRightSpace:
        return _T("icon_image_right_space");
    case Id::kIndent:
        return _T("indent");
    case Id::kExpandImageClass:
        return _T("expand_image_class");
    case Id::kShowIcon:
        return _T("show_icon");
    case Id::kMouseChild:
        return _T("mouse_child");
    case Id::kDragOutId:
        return _T("drag_out_id");
    case Id::kDropInId:
        return _T("drop_in_id");
    case Id::kClass:
        return _T("class");
    case Id::kEnableVars:
        return _T("enable_vars");
    case Id::kHalign:
        return _T("halign");
    case Id::kValign:
        return _T("valign");
    case Id::kAlign:
        return _T("align");
    case Id::kMargin:
        return _T("margin");
    case Id::kPadding:
        return _T("padding");
    case Id::kControlPadding:
        return _T("control_padding");
    case Id::kBkcolor:
        return _T("bkcolor");
    case Id::kBkcolor2:
        return _T("bkcolor2");
    case Id::kBkcolor2Direction:
        return _T("bkcolor2_direction");
    case Id::kForeColor:
        return _T("fore_color");
    case Id::kBorderSize:
        return _T("border_size");
    case Id::kBorderDashStyle:
        return _T("border_dash_style");
    case Id::kBordersOnTop:
        return _T("borders_on_top");
    case Id::kBorderRound:
        return _T("border_round");
    case Id::kBoxShadow:
        return _T("box_shadow");
    case Id::kWidth:
        return _T("width");
    case Id::kState:
        return _T("state");
    case Id::kCursorType:
        return _T("cursor_type");
    case Id::kRenderOffset:
        return _T("render_offset");
    case Id::kNormalColor:
        return _T("normal_color");
    case Id::kHoveredColor:
        return _T("hovered_color");
    case Id::kHotColor:
        return _T("hot_color");
    case Id::kPressedColor:
        return _T("pressed_color");
    case Id::kPushedColor:
        return _T("pushed_color");
    case Id::kDisabledColor:
        return _T("disabled_color");
    case Id::kNormalColorMargin:
        return _T("normal_color_margin");
    case Id::kHoveredColorMargin:
        return _T("hovered_color_margin");
    case Id::kHotColorMargin:
        return _T("hot_color_margin");
    case Id::kPressedColorMargin:
        return _T("pressed_color_margin");
    case Id::kPushedColorMargin:
        return _T("pushed_color_margin");
    case Id::kDisabledColorMargin:
        return _T("disabled_color_margin");
    case Id::kStateColorMinWidth:
        return _T("state_color_min_width");
    case Id::kStateColorMinHeight:
        return _T("state_color_min_height");
    case Id::kNormalColorRound:
        return _T("normal_color_round");
    case Id::kHoveredColorRound:
        return _T("hovered_color_round");
    case Id::kHotColorRound:
        return _T("hot_color_round");
    case Id::kPressedColorRound:
        return _T("pressed_color_round");
    case Id::kPushedColorRound:
        return _T("pushed_color_round");
    case Id::kDisabledColorRound:
        return _T("disabled_color_round");
    case Id::kBorderColor:
        return _T("border_color");
    case Id::kNormalBorderColor:
        return _T("normal_border_color");
    case Id::kHoveredBorderColor:
        return _T("hovered_border_color");
    case Id::kHotBorderColor:
        return _T("hot_border_color");
    case Id::kPressedBorderColor:
        return _T("pressed_border_color");
    case Id::kPushedBorderColor:
        return _T("pushed_border_color");
    case Id::kDisabledBorderColor:
        return _T("disabled_border_color");
    case Id::kFocusedBorderColor:
        return _T("focused_border_color");
    case Id::kFocusBorderColor:
        return _T("focus_border_color");
    case Id::kLeftBorderSize:
        return _T("left_border_size");
    case Id::kTopBorderSize:
        return _T("top_border_size");
    case Id::kRightBorderSize:
        return _T("right_border_size");
    case Id::kBottomBorderSize:
        return _T("bottom_border_size");
    case Id::kBkimage:
        return _T("bkimage");
    case Id::kMinWidth:
        return _T("min_width");
    case Id::kMaxWidth:
        return _T("max_width");
    case Id::kMinHeight:
        return _T("min_height");
    case Id::kMaxHeight:
        return _T("max_height");
    case Id::kName:
        return _T("name");
    case Id::kTooltipText:
        return _T("tooltip_text");
    case Id::kTooltipTextId:
        return _T("tooltip_text_id");
    case Id::kTooltipTextid:
        return _T("tooltip_textid");
    case Id::kTooltiptextid:
        return _T("tooltiptextid");
    case Id::kTooltipWidth:
        return _T("tooltip_width");
    case Id::kDataId:
        return _T("data_id");
    case Id::kUserDataId:
        return _T("user_data_id");
    case Id::kUserDataid:
        return _T("user_dataid");
    case Id::kEnabled:
        return _T("enabled");
    case Id::kMouseEnabled:
        return _T("mouse_enabled");
    case Id::kMouse:
        return _T("mouse");
    case Id::kKeyboardEnabled:
        return _T("keyboard_enabled");
    case Id::kKeyboard:
        return _T("keyboard");
    case Id::kVisible:
        return _T("visible");
    case Id::kFadeVisible:
        return _T("fade_visible");
    case Id::kFloat:
        return _T("float");
    case Id::kKeepFloatPos:
        return _T("keep_float_pos");
    case Id::kCache:
        return _T("cache");
    case Id::kNoFocus:
        return _T("no_focus");
    case Id::kAlpha:
        return _T("alpha");
    case Id::kNormalImage:
        return _T("normal_image");
    case Id::kHoveredImage:
        return _T("hovered_image");
    case Id::kHotImage:
        return _T("hot_image");
    case Id::kPressedImage:
        return _T("pressed_image");
    case Id::kPushedImage:
        return _T("pushed_image");
    case Id::kDisabledImage:
        return _T("disabled_image");
    case Id::kForeNormalImage:
        return _T("fore_normal_image");
    case Id::kForeHoveredImage:
        return _T("fore_hovered_image");
    case Id::kForeHotImage:
        return _T("fore_hot_image");
    case Id::kForePressedImage:
        return _T("fore_pressed_image");
    case Id::kForePushedImage:
        return _T("fore_pushed_image");
    case Id::kForeDisabledImage:
        return _T("fore_disabled_image");
    case Id::kFadeAlpha:
        return _T("fade_alpha");
    case Id::kFadeHovered:
        return _T("fade_hovered");
    case Id::kFadeHot:
        return _T("fade_hot");
    case Id::kFadeHoveredFrameIntervalMs:
        return _T("fade_hovered_frame_interval_ms");
    case Id::kFadeHotFrameIntervalMs:
        return _T("fade_hot_frame_interval_ms");
    case Id::kFadeHoveredTotalMs:
        return _T("fade_hovered_total_ms");
    case Id::kFadeHotTotalMs:
        return _T("fade_hot_total_ms");
    case Id::kFadeHoveredEasingFunction:
        return _T("fade_hovered_easing_function");
    case Id::kFadeHotEasingFunction:
        return _T("fade_hot_easing_function");
    case Id::kFadeWidth:
        return _T("fade_width");
    case Id::kFadeHeight:
        return _T("fade_height");
    case Id::kFadeSize:
        return _T("fade_size");
    case Id::kFadeInOutXFromLeft:
        return _T("fade_in_out_x_from_left");
    case Id::kFadeInOutXFromRight:
        return _T("fade_in_out_x_from_right");
    case Id::kFadeInOutYFromTop:
        return _T("fade_in_out_y_from_top");
    case Id::kFadeInOutYFromBottom:
        return _T("fade_in_out_y_from_bottom");
    case Id::kFadeFrameIntervalMs:
        return _T("fade_frame_interval_ms");
    case Id::kFadeTotalMs:
        return _T("fade_total_ms");
    case Id::kFadeEasingFunction:
        return _T("fade_easing_function");
    case Id::kTabStop:
        return _T("tab_stop");
    case Id::kLoading:
        return _T("loading");
    case Id::kShowFocusedRect:
        return _T("show_focused_rect");
    case Id::kShowFocusRect:
        return _T("show_focus_rect");
    case Id::kFocusedRectColor:
        return _T("focused_rect_color");
    case Id::kFocusRectColor:
        return _T("focus_rect_color");
    case Id::kPaintOrder:
        return _T("paint_order");
    case Id::kStartImageAnimation:
        return _T("start_image_animation");
    case Id::kStartGifPlay:
        return _T("start_gif_play");
    case Id::kStopImageAnimation:
        return _T("stop_image_animation");
    case Id::kStopGifPlay:
        return _T("stop_gif_play");
    case Id::kSetImageAnimationFrame:
        return _T("set_image_animation_frame");
    case Id::kEnableDragDrop:
        return _T("enable_drag_drop");
    case Id::kEnableDropFile:
        return _T("enable_drop_file");
    case Id::kDropFileTypes:
        return _T("drop_file_types");
    case Id::kRowSpan:
        return _T("row_span");
    case Id::kColSpan:
        return _T("col_span");
    case Id::kButton1NormalImage:
        return _T("button1_normal_image");
    case Id::kButton1HoveredImage:
        return _T("button1_hovered_image");
    case Id::kButton1HotImage:
        return _T("button1_hot_image");
    case Id::kButton1PressedImage:
        return _T("button1_pressed_image");
    case Id::kButton1PushedImage:
        return _T("button1_pushed_image");
    case Id::kButton1DisabledImage:
        return _T("button1_disabled_image");
    case Id::kButton2NormalImage:
        return _T("button2_normal_image");
    case Id::kButton2HoveredImage:
        return _T("button2_hovered_image");
    case Id::kButton2HotImage:
        return _T("button2_hot_image");
    case Id::kButton2PressedImage:
        return _T("button2_pressed_image");
    case Id::kButton2PushedImage:
        return _T("button2_pushed_image");
    case Id::kButton2DisabledImage:
        return _T("button2_disabled_image");
    case Id::kRailNormalImage:
        return _T("rail_normal_image");
    case Id::kRailHoveredImage:
        return _T("rail_hovered_image");
    case Id::kRailHotImage:
        return _T("rail_hot_image");
    case Id::kRailPressedImage:
        return _T("rail_pressed_image");
    case Id::kRailPushedImage:
        return _T("rail_pushed_image");
    case Id::kRailDisabledImage:
        return _T("rail_disabled_image");
    case Id::kBkNormalImage:
        return _T("bk_normal_image");
    case Id::kBkHoveredImage:
        return _T("bk_hovered_image");
    case Id::kBkHotImage:
        return _T("bk_hot_image");
    case Id::kBkPressedImage:
        return _T("bk_pressed_image");
    case Id::kBkPushedImage:
        return _T("bk_pushed_image");
    case Id::kBkDisabledImage:
        return _T("bk_disabled_image");
    case Id::kLineSize:
        return _T("line_size");
    case Id::kThumbMinLength:
        return _T("thumb_min_length");
    case Id::kRange:
        return _T("range");
    case Id::kShowButton1:
        return _T("show_button1");
    case Id::kShowButton2:
        return _T("show_button2");
    case Id::kAutoHideScroll:
        return _T("auto_hide_scroll");
    case Id::kSelected:
        return _T("selected");
    case Id::kSwitchSelect:
        return _T("switch_select");
    case Id::kSupportCheckMode:
        return _T("support_check_Mode");
    case Id::kAutoSelectCheck:
        return _T("auto_select_check");
    case Id::kNormalFirst:
        return _T("normal_first");
    case Id::kSelectedNormalImage:
        return _T("selected_normal_image");
    case Id::kSelectedHoveredImage:
        return _T("selected_hovered_image");
    case Id::kSelectedHotImage:
        return _T("selected_hot_image");
    case Id::kSelectedPressedImage:
        return _T("selected_pressed_image");
    case Id::kSelectedPushedImage:
        return _T("selected_pushed_image");
    case Id::kSelectedDisabledImage:
        return _T("selected_disabled_image");
    case Id::kSelectedForeNormalImage:
        return _T("selected_fore_normal_image");
    case Id::kSelectedForeHoveredImage:
        return _T("selected_fore_hovered_image");
    case Id::kSelectedForeHotImage:
        return _T("selected_fore_hot_image");
    case Id::kSelectedForePressedImage:
        return _T("selected_fore_pressed_image");
    case Id::kSelectedForePushedImage:
        return _T("selected_fore_pushed_image");
    case Id::kSelectedForeDisabledImage:
        return _T("selected_fore_disabled_image");
    case Id::kPartSelectedNormalImage:
        return _T("part_selected_normal_image");
    case Id::kPartSelectedHoveredImage:
        return _T("part_selected_hovered_image");
    case Id::kPartSelectedHotImage:
        return _T("part_selected_hot_image");
    case Id::kPartSelectedPressedImage:
        return _T("part_selected_pressed_image");
    case Id::kPartSelectedPushedImage:
        return _T("part_selected_pushed_image");
    case Id::kPartSelectedDisabledImage:
        return _T("part_selected_disabled_image");
    case Id::kPartSelectedForeNormalImage:
        return _T("part_selected_fore_normal_image");
    case Id::kPartSelectedForeHoveredImage:
        return _T("part_selected_fore_hovered_image");
    case Id::kPartSelectedForeHotImage:
        return _T("part_selected_fore_hot_image");
    case Id::kPartSelectedForePressedImage:
        return _T("part_selected_fore_pressed_image");
    case Id::kPartSelectedForePushedImage:
        return _T("part_selected_fore_pushed_image");
    case Id::kPartSelectedForeDisabledImage:
        return _T("part_selected_fore_disabled_image");
    case Id::kSelectedTextColor:
        return _T("selected_text_color");
    case Id::kSelectedNormalTextColor:
        return _T("selected_normal_text_color");
    case Id::kSelectedHoveredTextColor:
        return _T("selected_hovered_text_color");
    case Id::kSelectedHotTextColor:
        return _T("selected_hot_text_color");
    case Id::kSelectedPressedTextColor:
        return _T("selected_pressed_text_color");
    case Id::kSelectedPushedTextColor:
        return _T("selected_pushed_text_color");
    case Id::kSelectedDisabledTextColor:
        return _T("selected_disabled_text_color");
    case Id::kSelectedNormalColor:
        return _T("selected_normal_color");
    case Id::kSelectedHoveredColor:
        return _T("selected_hovered_color");
    case Id::kSelectedHotColor:
        return _T("selected_hot_color");
    case Id::kSelectedPressedColor:
        return _T("selected_pressed_color");
    case Id::kSelectedPushedColor:
        return _T("selected_pushed_color");
    case Id::kSelectedDisabledColor:
        return _T("selected_disabled_color");
    case Id::kSelectedNormalColorMargin:
        return _T("selected_normal_color_margin");
    case Id::kSelectedHoveredColorMargin:
        return _T("selected_hovered_color_margin");
    case Id::kSelectedHotColorMargin:
        return _T("selected_hot_color_margin");
    case Id::kSelectedPressedColorMargin:
        return _T("selected_pressed_color_margin");
    case Id::kSelectedPushedColorMargin:
        return _T("selected_pushed_color_margin");
    case Id::kSelectedDisabledColorMargin:
        return _T("selected_disabled_color_margin");
    case Id::kSelectedNormalColorRound:
        return _T("selected_normal_color_round");
    case Id::kSelectedHoveredColorRound:
        return _T("selected_hovered_color_round");
    case Id::kSelectedHotColorRound:
        return _T("selected_hot_color_round");
    case Id::kSelectedPressedColorRound:
        return _T("selected_pressed_color_round");
    case Id::kSelectedPushedColorRound:
        return _T("selected_pushed_color_round");
    case Id::kSelectedDisabledColorRound:
        return _T("selected_disabled_color_round");
    case Id::kCornerSize:
        return _T("corner_size");
    case Id::kUrl:
        return _T("url");
    case Id::kShowUrlTooltip:
        return _T("show_url_tooltip");
    case Id::kGroup:
        return _T("group");
    case Id::kEnableSplitSingle:
        return _T("enable_split_single");
    case Id::kDragAlpha:
        return _T("drag_alpha");
    case Id::kDragOut:
        return _T("drag_out");
    case Id::kEnableMovePos:
        return _T("enable_move_pos");
    case Id::kMoveParentPos:
        return _T("move_parent_pos");
    case Id::kMovePosAlpha:
        return _T("move_pos_alpha");
    case Id::kMovePosNonDraggableMargin:
        return _T("move_pos_non_draggable_margin");
    case Id::kMovePosDraggableBorder:
        return _T("move_pos_draggable_border");
    case Id::kMovePosReserveWidth:
        return _T("move_pos_reserve_width");
    case Id::kMovePosReserveHeight:
        return _T("move_pos_reserve_height");
    case Id::kMovePosKeepWithinParent:
        return _T("move_pos_keep_within_parent");
    case Id::kEnableResize:
        return _T("enable_resize");
    case Id::kResizeSizeBox:
        return _T("resize_size_box");
    case Id::kResizeReserveWidth:
        return _T("resize_reserve_width");
    case Id::kResizeReserveHeight:
        return _T("resize_reserve_height");
    case Id::kResizeKeepWithinParent:
        return _T("resize_keep_within_parent");
    case Id::kUrlIsLocalFile:
        return _T("url_is_local_file");
    case Id::kF12:
        return _T("F12");
    case Id::kF11:
        return _T("F11");
    case Id::kDownloadFaviconImage:
        return _T("download_favicon_image");
    case Id::kDevtoolsEnabled:
        return _T("devtools_enabled");
    case Id::kRows:
        return _T("rows");
    case Id::kColumns:
        return _T("columns");
    case Id::kGridWidth:
        return _T("grid_width");
    case Id::kGridHeight:
        return _T("grid_height");
    case Id::kScaleDown:
        return _T("scale_down");
    case Id::kItemSize:
        return _T("item_size");
    case Id::kAutoCalcItemSize:
        return _T("auto_calc_item_size");
    case Id::kChildMargin:
        return _T("child_margin");
    case Id::kChildMarginX:
        return _T("child_margin_x");
    case Id::kChildMarginY:
        return _T("child_margin_y");
    case Id::kChildValign:
        return _T("child_valign");
    case Id::kChildHalign:
        return _T("child_halign");
    case Id::kChildAlign:
        return _T("child_align");
    default:
        break;
    }
    return DString();
}
} //namespace control

namespace ctrl {
Id IdOf(const DString &strName)
{
    switch (Hash(strName)) {
    case 0x8347BF3Eu: //Control
        return Id::kControl;
    case 0x54047E30u: //ControlDragable
        return Id::kControlDragable;
    case 0x41390E74u: //ControlMovable
        return Id::kControlMovable;
    case 0xDAC3CD9Fu: //ControlResizable
        return Id::kControlResizable;
    case 0x9ECCF29Du: //Label
        return Id::kLabel;
    case 0x641DAC20u: //RichText
        return Id::kRichText;
    case 0x013F0DADu: //HyperLink
        return Id::kHyperLink;
    case 0x33881A91u: //Button
        return Id::kButton;
    case 0x17245CB4u: //Option
        return Id::kOption;
    case 0x9D7F5B98u: //CheckBox
        return Id::kCheckBox;
    case 0x16A15383u: //Split
        return Id::kSplit;
    case 0xA109AC9Au: //LabelBox
        return Id::kLabelBox;
    case 0xDD94BF10u: //LabelHBox
        return Id::kLabelHBox;
    case 0xC402517Eu: //LabelVBox
        return Id::kLabelVBox;
    case 0xC9FECC89u: //RichTextBox
        return Id::kRichTextBox;
    case 0xB78E1A11u: //RichTextHBox
        return Id::kRichTextHBox;
    case 0xC34A649Fu: //RichTextVBox
        return Id::kRichTextVBox;
    case 0xDD7B687Eu: //ButtonBox
        return Id::kButtonBox;
    case 0x12AD9B6Cu: //ButtonHBox
        return Id::kButtonHBox;
    case 0x3FEF4752u: //ButtonVBox
        return Id::kButtonVBox;
    case 0x2D942DB1u: //CheckBoxBox
        return Id::kCheckBoxBox;
    case 0xD3450A59u: //CheckBoxHBox
        return Id::kCheckBoxHBox;
    case 0xAFE5BF57u: //CheckBoxVBox
        return Id::kCheckBoxVBox;
    case 0x0776F47Du: //OptionBox
        return Id::kOptionBox;
    case 0x28E2207Du: //OptionHBox
        return Id::kOptionHBox;
    case 0x0D3E6633u: //OptionVBox
        return Id::kOptionVBox;
    case 0x99FFD964u: //SplitBox
        return Id::kSplitBox;
    case 0xD5CB7ED2u: //Box
        return Id::kBox;
    case 0x875B9968u: //HBox
        return Id::kHBox;
    case 0x9D80FF26u: //VBox
        return Id::kVBox;
    case 0x281E36F4u: //HFlowBox
        return Id::kHFlowBox;
    case 0x9D3B7BC6u: //VFlowBox
        return Id::kVFlowBox;
    case 0xC1BE0443u: //XmlBox
        return Id::kXmlBox;
    case 0x629420C2u: //VTileBox
        return Id::kVTileBox;
    case 0xB5258824u: //HTileBox
        return Id::kHTileBox;
    case 0x4C89B034u: //BoxDragable
        return Id::kBoxDragable;
    case 0x0005D45Eu: //HBoxDragable
        return Id::kHBoxDragable;
    case 0x3CC3CCA8u: //VBoxDragable
        return Id::kVBoxDragable;
    case 0xC22053D8u: //BoxMovable
        return Id::kBoxMovable;
    case 0x0940F646u: //HBoxMovable
        return Id::kHBoxMovable;
    case 0xE4D4523Cu: //VBoxMovable
        return Id::kVBoxMovable;
    case 0xD8B33073u: //BoxResizable
        return Id::kBoxResizable;
    case 0x389E7975u: //HBoxResizable
        return Id::kHBoxResizable;
    case 0x919AD8A7u: //VBoxResizable
        return Id::kVBoxResizable;
    case 0xCB3A94A5u: //GroupBox
        return Id::kGroupBox;
    case 0xAA9C22E5u: //GroupHBox
        return Id::kGroupHBox;
    case 0x5F843ACBu: //GroupVBox
        return Id::kGroupVBox;
    case 0x1881EB85u: //ScrollBox
        return Id::kScrollBox;
    case 0x69DE0C73u: //HScrollBox
        return Id::kHScrollBox;
    case 0x4A3BF581u: //VScrollBox
        return Id::kVScrollBox;
    case 0x6623F1D7u: //HFlowScrollBox
        return Id::kHFlowScrollBox;
    case 0x8F06DD61u: //VFlowScrollBox
        return Id::kVFlowScrollBox;
    case 0x71D089C7u: //HTileScrollBox
        return Id::kHTileScrollBox;
    case 0xF284FF35u: //VTileScrollBox
        return Id::kVTileScrollBox;
    case 0xCFB2280Du: //ListBoxItem
        return Id::kListBoxItem;
    case 0x3A75649Fu: //ListBoxItemH
        return Id::kListBoxItemH;
    case 0x50758741u: //ListBoxItemV
        return Id::kListBoxItemV;
    case 0xBD1880E0u: //HListBox
        return Id::kHListBox;
    case 0x6380F3A2u: //VListBox
        return Id::kVListBox;
    case 0xBAD0D5FCu: //HTileListBox
        return Id::kHTileListBox;
    case 0x367F1ABEu: //VTileListBox
        return Id::kVTileListBox;
    case 0x3D728820u: //ListCtrl
        return Id::kListCtrl;
    case 0xAA7EC5E8u: //PropertyGrid
        return Id::kPropertyGrid;
    case 0x25FE29F7u: //VirtualVListBox
        return Id::kVirtualVListBox;
    case 0x2BF812CDu: //VirtualHListBox
        return Id::kVirtualHListBox;
    case 0x418A8459u: //VirtualHTileListBox
        return Id::kVirtualHTileListBox;
    case 0x8D30DE43u: //VirtualVTileListBox
        return Id::kVirtualVTileListBox;
    case 0x1FD60165u: //TabBox
        return Id::kTabBox;
    case 0xE9C414DEu: //GridBox
        return Id::kGridBox;
    case 0x623E2129u: //GridScrollBox
        return Id::kGridScrollBox;
    case 0x1B2D7B5Du: //TreeNode
        return Id::kTreeNode;
    case 0x04FF2448u: //TreeView
        return Id::kTreeView;
    case 0xE16143B0u: //DirectoryTree
        return Id::kDirectoryTree;
    case 0x92130E4Bu: //RichEdit
        return Id::kRichEdit;
    case 0x6CFFC87Bu: //RichEdit2
        return Id::kRichEdit2;
    case 0x83CD7FA1u: //Combo
        return Id::kCombo;
    case 0xDF76ACF5u: //ComboButton
        return Id::kComboButton;
    case 0xB256479Fu: //FilterCombo
        return Id::kFilterCombo;
    case 0x0F4085CFu: //CheckCombo
        return Id::kCheckCombo;
    case 0x9B74C01Cu: //Slider
        return Id::kSlider;
    case 0x1A36A166u: //Progress
        return Id::kProgress;
    case 0x4A4C6722u: //CircleProgress
        return Id::kCircleProgress;
    case 0x0690D9B9u: //ScrollBar
        return Id::kScrollBar;
    case 0x00C6075Au: //Menu
        return Id::kMenu;
    case 0x7487F36Au: //SubMenu
        return Id::kSubMenu;
    case 0xEAED44D1u: //MenuItem
        return Id::kMenuItem;
    case 0xCEF80263u: //MenuListBox
        return Id::kMenuListBox;
    case 0xB43DB67Bu: //MenuBar
        return Id::kMenuBar;
    case 0x7362A2BCu: //MenuBarItem
        return Id::kMenuBarItem;
    case 0x9BEC7490u: //DateTime
        return Id::kDateTime;
    case 0x5E3580C2u: //CefControl
        return Id::kCefControl;
    case 0xEA3FC8FFu: //WebView2Control
        return Id::kWebView2Control;
    case 0x68FF0BCDu: //ColorControl
        return Id::kColorControl;
    case 0xF376D9FDu: //ColorSlider
        return Id::kColorSlider;
    case 0xAC14FC3Eu: //ColorPickerRegular
        return Id::kColorPickerRegular;
    case 0x1857C9ABu: //ColorPickerStandard
        return Id::kColorPickerStandard;
    case 0x5C84F120u: //ColorPickerStandardGray
        return Id::kColorPickerStandardGray;
    case 0x96FBC991u: //ColorPickerCustom
        return Id::kColorPickerCustom;
    case 0x9808F547u: //Line
        return Id::kLine;
    case 0x8A17D3ECu: //IPAddress
        return Id::kIPAddress;
    case 0x180226F3u: //HotKey
        return Id::kHotKey;
    case 0xA4894655u: //TabCtrl
        return Id::kTabCtrl;
    case 0x0007F9F6u: //TabCtrlItem
        return Id::kTabCtrlItem;
    case 0x9981687Cu: //AddressBar
        return Id::kAddressBar;
    case 0xB189E975u: //IconControl
        return Id::kIconControl;
    case 0x782BB2ABu: //BitmapControl
        return Id::kBitmapControl;
    case 0x29BA0125u: //ChildWindow
        return Id::kChildWindow;
    case 0x7F16C2A2u: //window_title_bar
        return Id::kWindowTitleBar;
    case 0xB8037C4Fu: //btn_window_close
        return Id::kBtnWindowClose;
    case 0x176500FBu: //btn_window_min
        return Id::kBtnWindowMin;
    case 0x0D4FD685u: //btn_window_max
        return Id::kBtnWindowMax;
    case 0x21EC6B55u: //btn_window_restore
        return Id::kBtnWindowRestore;
    case 0x3EC82B7Cu: //btn_window_fullscreen
        return Id::kBtnWindowFullscreen;
    case 0x76E493D7u: //btn_window_language
        return Id::kBtnWindowLanguage;
    case 0x3945BAD6u: //btn_window_theme
        return Id::kBtnWindowTheme;
    default:
        break;
    }
    return Id::kInvalidId;
}

DString IdToString(Id id)
{
    switch (id) {
    case Id::kControl:
        return _T("Control");
    case Id::kControlDragable:
        return _T("ControlDragable");
    case Id::kControlMovable:
        return _T("ControlMovable");
    case Id::kControlResizable:
        return _T("ControlResizable");
    case Id::kLabel:
        return _T("Label");
    case Id::kRichText:
        return _T("RichText");
    case Id::kHyperLink:
        return _T("HyperLink");
    case Id::kButton:
        return _T("Button");
    case Id::kOption:
        return _T("Option");
    case Id::kCheckBox:
        return _T("CheckBox");
    case Id::kSplit:
        return _T("Split");
    case Id::kLabelBox:
        return _T("LabelBox");
    case Id::kLabelHBox:
        return _T("LabelHBox");
    case Id::kLabelVBox:
        return _T("LabelVBox");
    case Id::kRichTextBox:
        return _T("RichTextBox");
    case Id::kRichTextHBox:
        return _T("RichTextHBox");
    case Id::kRichTextVBox:
        return _T("RichTextVBox");
    case Id::kButtonBox:
        return _T("ButtonBox");
    case Id::kButtonHBox:
        return _T("ButtonHBox");
    case Id::kButtonVBox:
        return _T("ButtonVBox");
    case Id::kCheckBoxBox:
        return _T("CheckBoxBox");
    case Id::kCheckBoxHBox:
        return _T("CheckBoxHBox");
    case Id::kCheckBoxVBox:
        return _T("CheckBoxVBox");
    case Id::kOptionBox:
        return _T("OptionBox");
    case Id::kOptionHBox:
        return _T("OptionHBox");
    case Id::kOptionVBox:
        return _T("OptionVBox");
    case Id::kSplitBox:
        return _T("SplitBox");
    case Id::kBox:
        return _T("Box");
    case Id::kHBox:
        return _T("HBox");
    case Id::kVBox:
        return _T("VBox");
    case Id::kHFlowBox:
        return _T("HFlowBox");
    case Id::kVFlowBox:
        return _T("VFlowBox");
    case Id::kXmlBox:
        return _T("XmlBox");
    case Id::kVTileBox:
        return _T("VTileBox");
    case Id::kHTileBox:
        return _T("HTileBox");
    case Id::kBoxDragable:
        return _T("BoxDragable");
    case Id::kHBoxDragable:
        return _T("HBoxDragable");
    case Id::kVBoxDragable:
        return _T("VBoxDragable");
    case Id::kBoxMovable:
        return _T("BoxMovable");
    case Id::kHBoxMovable:
        return _T("HBoxMovable");
    case Id::kVBoxMovable:
        return _T("VBoxMovable");
    case Id::kBoxResizable:
        return _T("BoxResizable");
    case Id::kHBoxResizable:
        return _T("HBoxResizable");
    case Id::kVBoxResizable:
        return _T("VBoxResizable");
    case Id::kGroupBox:
        return _T("GroupBox");
    case Id::kGroupHBox:
        return _T("GroupHBox");
    case Id::kGroupVBox:
        return _T("GroupVBox");
    case Id::kScrollBox:
        return _T("ScrollBox");
    case Id::kHScrollBox:
        return _T("HScrollBox");
    case Id::kVScrollBox:
        return _T("VScrollBox");
    case Id::kHFlowScrollBox:
        return _T("HFlowScrollBox");
    case Id::kVFlowScrollBox:
        return _T("VFlowScrollBox");
    case Id::kHTileScrollBox:
        return _T("HTileScrollBox");
    case Id::kVTileScrollBox:
        return _T("VTileScrollBox");
    case Id::kListBoxItem:
        return _T("ListBoxItem");
    case Id::kListBoxItemH:
        return _T("ListBoxItemH");
    case Id::kListBoxItemV:
        return _T("ListBoxItemV");
    case Id::kHListBox:
        return _T("HListBox");
    case Id::kVListBox:
        return _T("VListBox");
    case Id::kHTileListBox:
        return _T("HTileListBox");
    case Id::kVTileListBox:
        return _T("VTileListBox");
    case Id::kListCtrl:
        return _T("ListCtrl");
    case Id::kPropertyGrid:
        return _T("PropertyGrid");
    case Id::kVirtualVListBox:
        return _T("VirtualVListBox");
    case Id::kVirtualHListBox:
        return _T("VirtualHListBox");
    case Id::kVirtualHTileListBox:
        return _T("VirtualHTileListBox");
    case Id::kVirtualVTileListBox:
        return _T("VirtualVTileListBox");
    case Id::kTabBox:
        return _T("TabBox");
    case Id::kGridBox:
        return _T("GridBox");
    case Id::kGridScrollBox:
        return _T("GridScrollBox");
    case Id::kTreeNode:
        return _T("TreeNode");
    case Id::kTreeView:
        return _T("TreeView");
    case Id::kDirectoryTree:
        return _T("DirectoryTree");
    case Id::kRichEdit:
        return _T("RichEdit");
    case Id::kRichEdit2:
        return _T("RichEdit2");
    case Id::kCombo:
        return _T("Combo");
    case Id::kComboButton:
        return _T("ComboButton");
    case Id::kFilterCombo:
        return _T("FilterCombo");
    case Id::kCheckCombo:
        return _T("CheckCombo");
    case Id::kSlider:
        return _T("Slider");
    case Id::kProgress:
        return _T("Progress");
    case Id::kCircleProgress:
        return _T("CircleProgress");
    case Id::kScrollBar:
        return _T("ScrollBar");
    case Id::kMenu:
        return _T("Menu");
    case Id::kSubMenu:
        return _T("SubMenu");
    case Id::kMenuItem:
        return _T("MenuItem");
    case Id::kMenuListBox:
        return _T("MenuListBox");
    case Id::kMenuBar:
        return _T("MenuBar");
    case Id::kMenuBarItem:
        return _T("MenuBarItem");
    case Id::kDateTime:
        return _T("DateTime");
    case Id::kCefControl:
        return _T("CefControl");
    case Id::kWebView2Control:
        return _T("WebView2Control");
    case Id::kColorControl:
        return _T("ColorControl");
    case Id::kColorSlider:
        return _T("ColorSlider");
    case Id::kColorPickerRegular:
        return _T("ColorPickerRegular");
    case Id::kColorPickerStandard:
        return _T("ColorPickerStandard");
    case Id::kColorPickerStandardGray:
        return _T("ColorPickerStandardGray");
    case Id::kColorPickerCustom:
        return _T("ColorPickerCustom");
    case Id::kLine:
        return _T("Line");
    case Id::kIPAddress:
        return _T("IPAddress");
    case Id::kHotKey:
        return _T("HotKey");
    case Id::kTabCtrl:
        return _T("TabCtrl");
    case Id::kTabCtrlItem:
        return _T("TabCtrlItem");
    case Id::kAddressBar:
        return _T("AddressBar");
    case Id::kIconControl:
        return _T("IconControl");
    case Id::kBitmapControl:
        return _T("BitmapControl");
    case Id::kChildWindow:
        return _T("ChildWindow");
    case Id::kWindowTitleBar:
        return _T("window_title_bar");
    case Id::kBtnWindowClose:
        return _T("btn_window_close");
    case Id::kBtnWindowMin:
        return _T("btn_window_min");
    case Id::kBtnWindowMax:
        return _T("btn_window_max");
    case Id::kBtnWindowRestore:
        return _T("btn_window_restore");
    case Id::kBtnWindowFullscreen:
        return _T("btn_window_fullscreen");
    case Id::kBtnWindowLanguage:
        return _T("btn_window_language");
    case Id::kBtnWindowTheme:
        return _T("btn_window_theme");
    default:
        break;
    }
    return DString();
}
} //namespace ctrl

namespace image {
Id IdOf(const DString &strName)
{
    switch (Hash(strName)) {
    case 0xAAEA5743u: //file
        return Id::kFile;
    case 0x2FF46619u: //res
        return Id::kRes;
    case 0x8D39BDE6u: //name
        return Id::kName;
    case 0x95876E1Fu: //width
        return Id::kWidth;
    case 0xD5BDBB42u: //height
        return Id::kHeight;
    case 0xD33CE1C9u: //src
        return Id::kSrc;
    case 0x1BCF29D8u: //source
        return Id::kSource;
    case 0x244A2150u: //corner
        return Id::kCorner;
    case 0x4C1096E4u: //window_shadow_mode
        return Id::kWindowShadowMode;
    case 0xF9DDE169u: //dpi_scale
        return Id::kDpiScale;
    case 0x375B8732u: //dpiscale（别名，归一到 dpi_scale）
        return Id::kDpiScale;
    case 0x32CEFE5Cu: //dest_scale
        return Id::kDestScale;
    case 0xE48CD7C1u: //destscale（别名，归一到 dest_scale）
        return Id::kDestScale;
    case 0x9A032455u: //dest
        return Id::kDest;
    case 0xD7713C7Bu: //margin
        return Id::kMargin;
    case 0x809608B6u: //padding
        return Id::kPadding;
    case 0x4A28CB3Cu: //halign
        return Id::kHalign;
    case 0x5A9DCFE2u: //valign
        return Id::kValign;
    case 0xF16D8413u: //fade
        return Id::kFade;
    case 0x5D8B6DABu: //alpha
        return Id::kAlpha;
    case 0xAC51FF95u: //xtiled
        return Id::kXtiled;
    case 0xD18F6E39u: //full_xtiled
        return Id::kFullXtiled;
    case 0xDA08D534u: //fullxtiled（别名，归一到 full_xtiled）
        return Id::kFullXtiled;
    case 0x0B6044ECu: //ytiled
        return Id::kYtiled;
    case 0x6B78EB30u: //full_ytiled
        return Id::kFullYtiled;
    case 0x8F5D689Du: //fullytiled（别名，归一到 full_ytiled）
        return Id::kFullYtiled;
    case 0x28235E36u: //tiled_margin
        return Id::kTiledMargin;
    case 0x32417511u: //tiledmargin（别名，归一到 tiled_margin）
        return Id::kTiledMargin;
    case 0x8C519D49u: //tiled_margin_x
        return Id::kTiledMarginX;
    case 0x8B519BB6u: //tiled_margin_y
        return Id::kTiledMarginY;
    case 0x2744F36Du: //tiled_padding
        return Id::kTiledPadding;
    case 0xA17CD3B4u: //icon_size
        return Id::kIconSize;
    case 0x50CE2E41u: //iconsize（别名，归一到 icon_size）
        return Id::kIconSize;
    case 0x915FBCFCu: //icon_as_animation
        return Id::kIconAsAnimation;
    case 0x561AA8C2u: //icon_frame_delay
        return Id::kIconFrameDelay;
    case 0x3507517Fu: //pag_max_frame_rate
        return Id::kPagMaxFrameRate;
    case 0xDBD3742Bu: //play_count
        return Id::kPlayCount;
    case 0xB0F30942u: //playcount（别名，归一到 play_count）
        return Id::kPlayCount;
    case 0x6A70D161u: //auto_play
        return Id::kAutoPlay;
    case 0x71A5632Cu: //async_load
        return Id::kAsyncLoad;
    case 0x4CC66687u: //adaptive_dest_rect
        return Id::kAdaptiveDestRect;
    case 0xD2AA781Bu: //svg_replace_colors
        return Id::kSvgReplaceColors;
    case 0xA5655C7Bu: //assert
        return Id::kAssert;
    default:
        break;
    }
    return Id::kInvalidId;
}

DString IdToString(Id id)
{
    switch (id) {
    case Id::kFile:
        return _T("file");
    case Id::kRes:
        return _T("res");
    case Id::kName:
        return _T("name");
    case Id::kWidth:
        return _T("width");
    case Id::kHeight:
        return _T("height");
    case Id::kSrc:
        return _T("src");
    case Id::kSource:
        return _T("source");
    case Id::kCorner:
        return _T("corner");
    case Id::kWindowShadowMode:
        return _T("window_shadow_mode");
    case Id::kDpiScale:
        return _T("dpi_scale");
    case Id::kDestScale:
        return _T("dest_scale");
    case Id::kDest:
        return _T("dest");
    case Id::kMargin:
        return _T("margin");
    case Id::kPadding:
        return _T("padding");
    case Id::kHalign:
        return _T("halign");
    case Id::kValign:
        return _T("valign");
    case Id::kFade:
        return _T("fade");
    case Id::kAlpha:
        return _T("alpha");
    case Id::kXtiled:
        return _T("xtiled");
    case Id::kFullXtiled:
        return _T("full_xtiled");
    case Id::kYtiled:
        return _T("ytiled");
    case Id::kFullYtiled:
        return _T("full_ytiled");
    case Id::kTiledMargin:
        return _T("tiled_margin");
    case Id::kTiledMarginX:
        return _T("tiled_margin_x");
    case Id::kTiledMarginY:
        return _T("tiled_margin_y");
    case Id::kTiledPadding:
        return _T("tiled_padding");
    case Id::kIconSize:
        return _T("icon_size");
    case Id::kIconAsAnimation:
        return _T("icon_as_animation");
    case Id::kIconFrameDelay:
        return _T("icon_frame_delay");
    case Id::kPagMaxFrameRate:
        return _T("pag_max_frame_rate");
    case Id::kPlayCount:
        return _T("play_count");
    case Id::kAutoPlay:
        return _T("auto_play");
    case Id::kAsyncLoad:
        return _T("async_load");
    case Id::kAdaptiveDestRect:
        return _T("adaptive_dest_rect");
    case Id::kSvgReplaceColors:
        return _T("svg_replace_colors");
    case Id::kAssert:
        return _T("assert");
    default:
        break;
    }
    return DString();
}
} //namespace image

namespace loading {
Id IdOf(const DString &strName)
{
    switch (Hash(strName)) {
    case 0xAAEA5743u: //file
        return Id::kFile;
    case 0x95876E1Fu: //width
        return Id::kWidth;
    case 0xD5BDBB42u: //height
        return Id::kHeight;
    case 0x685E6DD5u: //offset_x
        return Id::kOffsetX;
    case 0x675E6C42u: //offset_y
        return Id::kOffsetY;
    case 0x4A28CB3Cu: //halign
        return Id::kHalign;
    case 0x5A9DCFE2u: //valign
        return Id::kValign;
    case 0xF16D8413u: //fade
        return Id::kFade;
    case 0x2300C8E3u: //auto_stop
        return Id::kAutoStop;
    case 0xF6C3A76Bu: //animation_control
        return Id::kAnimationControl;
    default:
        break;
    }
    return Id::kInvalidId;
}

DString IdToString(Id id)
{
    switch (id) {
    case Id::kFile:
        return _T("file");
    case Id::kWidth:
        return _T("width");
    case Id::kHeight:
        return _T("height");
    case Id::kOffsetX:
        return _T("offset_x");
    case Id::kOffsetY:
        return _T("offset_y");
    case Id::kHalign:
        return _T("halign");
    case Id::kValign:
        return _T("valign");
    case Id::kFade:
        return _T("fade");
    case Id::kAutoStop:
        return _T("auto_stop");
    case Id::kAnimationControl:
        return _T("animation_control");
    default:
        break;
    }
    return DString();
}
} //namespace loading

namespace node {
Id IdOf(const DString &strName)
{
    switch (Hash(strName)) {
    case 0xE78631FDu: //Window
        return Id::kWindow;
    case 0x641280CEu: //Global
        return Id::kGlobal;
    case 0x50F97D9Fu: //Class
        return Id::kClass;
    case 0xCE311705u: //ThemeColor
        return Id::kThemeColor;
    case 0x96A9A1E1u: //TextColor
        return Id::kTextColor;
    case 0xA77A5EB0u: //Font
        return Id::kFont;
    case 0x9B626A65u: //DefaultFontFamilyNames
        return Id::kDefaultFontFamilyNames;
    case 0xCD2A0C12u: //FallbackFontFamilyNames
        return Id::kFallbackFontFamilyNames;
    case 0xEB7927A6u: //FontFile
        return Id::kFontFile;
    case 0x22E1F2F7u: //Alias
        return Id::kAlias;
    case 0xECB2125Eu: //Var
        return Id::kVar;
    case 0x4EAF7C92u: //Theme
        return Id::kTheme;
    case 0x7A005124u: //FontResource
        return Id::kFontResource;
    case 0x04E9AB9Fu: //ThemeMeta
        return Id::kThemeMeta;
    case 0xA6025607u: //Include
        return Id::kInclude;
    case 0x4306A7EBu: //PropertyGridGroup
        return Id::kPropertyGridGroup;
    case 0x192A8F98u: //CheckComboText
        return Id::kCheckComboText;
    case 0x921FCFB6u: //ListCtrlHeaderItem
        return Id::kListCtrlHeaderItem;
    case 0x72AB306Fu: //ListCtrlItem
        return Id::kListCtrlItem;
    case 0x57C087D9u: //ListCtrlSubItem
        return Id::kListCtrlSubItem;
    case 0xD02B533Fu: //Event
        return Id::kEvent;
    case 0xF4B5650Bu: //BubbledEvent
        return Id::kBubbledEvent;
    case 0xF8B142BEu: //PropertyGridTextProperty
        return Id::kPropertyGridTextProperty;
    case 0x75958EFBu: //PropertyGridComboProperty
        return Id::kPropertyGridComboProperty;
    case 0x6F064ED8u: //PropertyGridFontProperty
        return Id::kPropertyGridFontProperty;
    case 0x31813C7Du: //PropertyGridFontSizeProperty
        return Id::kPropertyGridFontSizeProperty;
    case 0x148B598Eu: //PropertyGridColorProperty
        return Id::kPropertyGridColorProperty;
    case 0xC56497C0u: //PropertyGridDateTimeProperty
        return Id::kPropertyGridDateTimeProperty;
    case 0x1CA97816u: //PropertyGridIPAddressProperty
        return Id::kPropertyGridIPAddressProperty;
    case 0xB0168387u: //PropertyGridHotKeyProperty
        return Id::kPropertyGridHotKeyProperty;
    case 0x94D6DA2Bu: //PropertyGridFileProperty
        return Id::kPropertyGridFileProperty;
    case 0x2F046572u: //PropertyGridDirectoryProperty
        return Id::kPropertyGridDirectoryProperty;
    case 0xE40C292Cu: //a
        return Id::kA;
    case 0xE70C2DE5u: //b
        return Id::kB;
    case 0xEC0C35C4u: //i
        return Id::kI;
    case 0xCF598E4Au: //del
        return Id::kDel;
    case 0xF60C4582u: //s
        return Id::kS;
    case 0x25B4AF4Fu: //strike
        return Id::kStrike;
    case 0x98E98317u: //ins
        return Id::kIns;
    case 0xF00C3C10u: //u
        return Id::kU;
    case 0xCC4A7989u: //bgcolor
        return Id::kBgcolor;
    case 0x274E1290u: //font
        return Id::kFont2;
    case 0x4F2BC4B5u: //br
        return Id::kBr;
    default:
        break;
    }
    return Id::kInvalidId;
}

DString IdToString(Id id)
{
    switch (id) {
    case Id::kWindow:
        return _T("Window");
    case Id::kGlobal:
        return _T("Global");
    case Id::kClass:
        return _T("Class");
    case Id::kThemeColor:
        return _T("ThemeColor");
    case Id::kTextColor:
        return _T("TextColor");
    case Id::kFont:
        return _T("Font");
    case Id::kDefaultFontFamilyNames:
        return _T("DefaultFontFamilyNames");
    case Id::kFallbackFontFamilyNames:
        return _T("FallbackFontFamilyNames");
    case Id::kFontFile:
        return _T("FontFile");
    case Id::kAlias:
        return _T("Alias");
    case Id::kVar:
        return _T("Var");
    case Id::kTheme:
        return _T("Theme");
    case Id::kFontResource:
        return _T("FontResource");
    case Id::kThemeMeta:
        return _T("ThemeMeta");
    case Id::kInclude:
        return _T("Include");
    case Id::kPropertyGridGroup:
        return _T("PropertyGridGroup");
    case Id::kCheckComboText:
        return _T("CheckComboText");
    case Id::kListCtrlHeaderItem:
        return _T("ListCtrlHeaderItem");
    case Id::kListCtrlItem:
        return _T("ListCtrlItem");
    case Id::kListCtrlSubItem:
        return _T("ListCtrlSubItem");
    case Id::kEvent:
        return _T("Event");
    case Id::kBubbledEvent:
        return _T("BubbledEvent");
    case Id::kPropertyGridTextProperty:
        return _T("PropertyGridTextProperty");
    case Id::kPropertyGridComboProperty:
        return _T("PropertyGridComboProperty");
    case Id::kPropertyGridFontProperty:
        return _T("PropertyGridFontProperty");
    case Id::kPropertyGridFontSizeProperty:
        return _T("PropertyGridFontSizeProperty");
    case Id::kPropertyGridColorProperty:
        return _T("PropertyGridColorProperty");
    case Id::kPropertyGridDateTimeProperty:
        return _T("PropertyGridDateTimeProperty");
    case Id::kPropertyGridIPAddressProperty:
        return _T("PropertyGridIPAddressProperty");
    case Id::kPropertyGridHotKeyProperty:
        return _T("PropertyGridHotKeyProperty");
    case Id::kPropertyGridFileProperty:
        return _T("PropertyGridFileProperty");
    case Id::kPropertyGridDirectoryProperty:
        return _T("PropertyGridDirectoryProperty");
    case Id::kA:
        return _T("a");
    case Id::kB:
        return _T("b");
    case Id::kI:
        return _T("i");
    case Id::kDel:
        return _T("del");
    case Id::kS:
        return _T("s");
    case Id::kStrike:
        return _T("strike");
    case Id::kIns:
        return _T("ins");
    case Id::kU:
        return _T("u");
    case Id::kBgcolor:
        return _T("bgcolor");
    case Id::kFont2:
        return _T("font");
    case Id::kBr:
        return _T("br");
    default:
        break;
    }
    return DString();
}
} //namespace node

namespace shadow {
Id IdOf(const DString &strName)
{
    switch (Hash(strName)) {
    case 0x53CACDB9u: //big
        return Id::kBig;
    case 0xC2AADE32u: //big_round
        return Id::kBigRound;
    case 0xA2C4F48Cu: //small
        return Id::kSmall;
    case 0xEB3E0D47u: //small_round
        return Id::kSmallRound;
    case 0x99E4DD3Au: //menu
        return Id::kMenu;
    case 0xF5E069FDu: //menu_round
        return Id::kMenuRound;
    case 0xADA7AFDBu: //none
        return Id::kNone;
    case 0xACC71EACu: //none_round
        return Id::kNoneRound;
    case 0x2057306Eu: //custom
        return Id::kCustom;
    case 0x933B5BDEu: //default
        return Id::kDefault;
    case 0x4DCD798Au: //system_default
        return Id::kSystemDefault;
    case 0xC4048C29u: //system_not_round
        return Id::kSystemNotRound;
    case 0x9D005E37u: //system_round
        return Id::kSystemRound;
    case 0x10830023u: //system_small_round
        return Id::kSystemSmallRound;
    default:
        break;
    }
    return Id::kInvalidId;
}

DString IdToString(Id id)
{
    switch (id) {
    case Id::kBig:
        return _T("big");
    case Id::kBigRound:
        return _T("big_round");
    case Id::kSmall:
        return _T("small");
    case Id::kSmallRound:
        return _T("small_round");
    case Id::kMenu:
        return _T("menu");
    case Id::kMenuRound:
        return _T("menu_round");
    case Id::kNone:
        return _T("none");
    case Id::kNoneRound:
        return _T("none_round");
    case Id::kCustom:
        return _T("custom");
    case Id::kDefault:
        return _T("default");
    case Id::kSystemDefault:
        return _T("system_default");
    case Id::kSystemNotRound:
        return _T("system_not_round");
    case Id::kSystemRound:
        return _T("system_round");
    case Id::kSystemSmallRound:
        return _T("system_small_round");
    default:
        break;
    }
    return DString();
}
} //namespace shadow

namespace window {
Id IdOf(const DString &strName)
{
    switch (Hash(strName)) {
    case 0xB8DEB7BDu: //use_system_caption
        return Id::kUseSystemCaption;
    case 0xA43C6CE2u: //shadow_attached
        return Id::kShadowAttached;
    case 0x36EF2581u: //shadowattached（别名，归一到 shadow_attached）
        return Id::kShadowAttached;
    case 0xB1B4685Au: //shadow_type
        return Id::kShadowType;
    case 0xFD4DBBA4u: //layered_window
        return Id::kLayeredWindow;
    case 0x488B50A7u: //layeredwindow（别名，归一到 layered_window）
        return Id::kLayeredWindow;
    case 0x5D8B6DABu: //alpha
        return Id::kAlpha;
    case 0x2C504187u: //layered_window_alpha
        return Id::kLayeredWindowAlpha;
    case 0xC6C2DD66u: //opacity
        return Id::kOpacity;
    case 0x9BDF0DBAu: //layered_window_opacity
        return Id::kLayeredWindowOpacity;
    case 0xC4FC7061u: //drag_drop
        return Id::kDragDrop;
    case 0x70648B17u: //render_backend_type
        return Id::kRenderBackendType;
    case 0x600E55D6u: //size_box
        return Id::kSizeBox;
    case 0xC49BC175u: //sizebox（别名，归一到 size_box）
        return Id::kSizeBox;
    case 0xEF131C65u: //caption
        return Id::kCaption;
    case 0xECA6B29Bu: //shadow_corner
        return Id::kShadowCorner;
    case 0x242356A4u: //shadowcorner（别名，归一到 shadow_corner）
        return Id::kShadowCorner;
    case 0x23A0D95Cu: //size
        return Id::kSize;
    case 0x77069CBCu: //size_contain_shadow
        return Id::kSizeContainShadow;
    case 0x756C9329u: //min_size
        return Id::kMinSize;
    case 0x503A58B3u: //mininfo
        return Id::kMininfo;
    case 0x6395123Bu: //max_size
        return Id::kMaxSize;
    case 0xC6C428C1u: //maxinfo
        return Id::kMaxinfo;
    case 0xA513A89Fu: //sdl_render_name
        return Id::kSdlRenderName;
    case 0x7AF5B662u: //snap_layout_menu
        return Id::kSnapLayoutMenu;
    case 0xD9350D5Cu: //sys_menu
        return Id::kSysMenu;
    case 0xCE4255DBu: //sys_menu_rect
        return Id::kSysMenuRect;
    case 0xE64015F0u: //icon
        return Id::kIcon;
    case 0xBDE64E3Eu: //text
        return Id::kText;
    case 0x7ACD9B7Eu: //text_id
        return Id::kTextId;
    case 0xC778FA43u: //textid（别名，归一到 text_id）
        return Id::kTextId;
    case 0xA6DF5631u: //round_corner
        return Id::kRoundCorner;
    case 0x49BBD5BAu: //roundcorner（别名，归一到 round_corner）
        return Id::kRoundCorner;
    case 0x27167A4Fu: //shadow_image
        return Id::kShadowImage;
    case 0x3036D436u: //shadowimage（别名，归一到 shadow_image）
        return Id::kShadowImage;
    case 0x7F7347B3u: //shadow_border_round
        return Id::kShadowBorderRound;
    case 0xC3DCEB84u: //shadow_border_size
        return Id::kShadowBorderSize;
    case 0xFB020660u: //shadow_border_color
        return Id::kShadowBorderColor;
    case 0x5E7A6BBEu: //shadow_snap
        return Id::kShadowSnap;
    case 0x8D39BDE6u: //name
        return Id::kName;
    case 0x425ED3CAu: //value
        return Id::kValue;
    case 0xAAEA5743u: //file
        return Id::kFile;
    case 0x83030020u: //desc
        return Id::kDesc;
    case 0x37386AE0u: //id
        return Id::kId;
    case 0xDE96F676u: //bold
        return Id::kBold;
    case 0xE582347Fu: //underline
        return Id::kUnderline;
    case 0xC2697BA1u: //strikeout
        return Id::kStrikeout;
    case 0x60E9FB6Du: //italic
        return Id::kItalic;
    case 0x933B5BDEu: //default
        return Id::kDefault;
    case 0x5127F14Du: //type
        return Id::kType;
    case 0x6AAC6670u: //receiver
        return Id::kReceiver;
    case 0x57647D0Cu: //apply_attribute
        return Id::kApplyAttribute;
    case 0xB98DDC81u: //applyattribute（别名，归一到 apply_attribute）
        return Id::kApplyAttribute;
    default:
        break;
    }
    return Id::kInvalidId;
}

DString IdToString(Id id)
{
    switch (id) {
    case Id::kUseSystemCaption:
        return _T("use_system_caption");
    case Id::kShadowAttached:
        return _T("shadow_attached");
    case Id::kShadowType:
        return _T("shadow_type");
    case Id::kLayeredWindow:
        return _T("layered_window");
    case Id::kAlpha:
        return _T("alpha");
    case Id::kLayeredWindowAlpha:
        return _T("layered_window_alpha");
    case Id::kOpacity:
        return _T("opacity");
    case Id::kLayeredWindowOpacity:
        return _T("layered_window_opacity");
    case Id::kDragDrop:
        return _T("drag_drop");
    case Id::kRenderBackendType:
        return _T("render_backend_type");
    case Id::kSizeBox:
        return _T("size_box");
    case Id::kCaption:
        return _T("caption");
    case Id::kShadowCorner:
        return _T("shadow_corner");
    case Id::kSize:
        return _T("size");
    case Id::kSizeContainShadow:
        return _T("size_contain_shadow");
    case Id::kMinSize:
        return _T("min_size");
    case Id::kMininfo:
        return _T("mininfo");
    case Id::kMaxSize:
        return _T("max_size");
    case Id::kMaxinfo:
        return _T("maxinfo");
    case Id::kSdlRenderName:
        return _T("sdl_render_name");
    case Id::kSnapLayoutMenu:
        return _T("snap_layout_menu");
    case Id::kSysMenu:
        return _T("sys_menu");
    case Id::kSysMenuRect:
        return _T("sys_menu_rect");
    case Id::kIcon:
        return _T("icon");
    case Id::kText:
        return _T("text");
    case Id::kTextId:
        return _T("text_id");
    case Id::kRoundCorner:
        return _T("round_corner");
    case Id::kShadowImage:
        return _T("shadow_image");
    case Id::kShadowBorderRound:
        return _T("shadow_border_round");
    case Id::kShadowBorderSize:
        return _T("shadow_border_size");
    case Id::kShadowBorderColor:
        return _T("shadow_border_color");
    case Id::kShadowSnap:
        return _T("shadow_snap");
    case Id::kName:
        return _T("name");
    case Id::kValue:
        return _T("value");
    case Id::kFile:
        return _T("file");
    case Id::kDesc:
        return _T("desc");
    case Id::kId:
        return _T("id");
    case Id::kBold:
        return _T("bold");
    case Id::kUnderline:
        return _T("underline");
    case Id::kStrikeout:
        return _T("strikeout");
    case Id::kItalic:
        return _T("italic");
    case Id::kDefault:
        return _T("default");
    case Id::kType:
        return _T("type");
    case Id::kReceiver:
        return _T("receiver");
    case Id::kApplyAttribute:
        return _T("apply_attribute");
    default:
        break;
    }
    return DString();
}
} //namespace window
} //namespace attr
} //namespace ui
