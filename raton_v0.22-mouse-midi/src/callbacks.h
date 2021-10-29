#include <gtk/gtk.h>


gboolean
on_ralphwindow_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_quit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_linearX_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_steppedX_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_randomX_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_sineX_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_customXuse_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_linearY_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_steppedY_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_randomY_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_sineY_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_customYuse_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_customX_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_customY_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
on_drawingarea1_button_press_event     (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_drawingarea1_button_release_event   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_drawingarea1_motion_notify_event    (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data);

void
on_drawingarea1_size_allocate          (GtkWidget       *widget,
                                        GdkRectangle    *allocation,
                                        gpointer         user_data);

gboolean
on_drawingarea1_key_press_event        (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data);

gboolean
on_drawingarea1_expose_event           (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

gboolean
on_drawingarea1_leave_notify_event     (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_drawingarea1_configure_event        (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data);

gboolean
on_drawingarea1_enter_notify_event     (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

void
on_controllerX_changed                 (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_invertx_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_reverseX_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_chanX_changed                       (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_controllerY_changed                 (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_invertY_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_reverseY_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_chanY_changed                       (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_dialog1_close                       (GtkDialog       *dialog,
                                        gpointer         user_data);

gboolean
on_dialog1_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_button1_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_curveX_curve_type_changed           (GtkCurve        *curve,
                                        gpointer         user_data);

gboolean
on_curveX_configure_event              (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data);

void
on_curveXsetlinear_clicked             (GtkButton       *button,
                                        gpointer         user_data);

void
on_curveXsetspline_clicked             (GtkButton       *button,
                                        gpointer         user_data);

void
on_curveXsetfree_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_curveXreset_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_curveXopen_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_curveXsaveas_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
on_cancelbutton_curveX_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_okbutton1_curveX_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_curveY_curve_type_changed           (GtkCurve        *curve,
                                        gpointer         user_data);

gboolean
on_curveY_configure_event              (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data);

void
on_customYsetlinear_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_curveYsetspline_clicked             (GtkButton       *button,
                                        gpointer         user_data);

void
on_curveYsetfree_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_curveYreset_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_curveYopen_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_curveYsaveas_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
on_cancelbutton_curveY_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_okbutton_curveY_clicked             (GtkButton       *button,
                                        gpointer         user_data);

void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_save_as1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_openfile_cancel                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_openfile_opened                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_savefile_cancel                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_savefile_save                       (GtkButton       *button,
                                        gpointer         user_data);

void
on_savefile_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_openfile_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_reset1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_shift_upX_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_shift_up_moreX_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_shift_downX_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_shift_down_moreX_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_compressX_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_comress_moreX_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_expandX_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_expand_moreX_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_shift_upY_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_shift_up_moreY_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_shift_downY_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_shift_down_moreY_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_compressY_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_compress_moreY_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_expandY_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_expand_moreY_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_preferences_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_stdfeedback_button_toggled          (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_colorfeedback_button_toggled        (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_prefsapply_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_prefscancel_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_prefsok_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_xpre_stepnumber_changed             (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_xpre_stepoffset_changed             (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_ypre_stepnumber_changed             (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_ypre_stepoffset_changed             (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_ypre_randnumber_changed             (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_xpre_randnumber_changed             (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_controllerXspin_changed             (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_controllerYspin_changed             (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_controllerXspin_value_changed       (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_chanX_value_changed                 (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_controllerYspin_value_changed       (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_chanY_value_changed                 (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_xpre_stepnumber_value_changed       (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_xpre_stepoffset_value_changed       (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_xpre_randnumber_value_changed       (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_ypre_stepnumber_value_changed       (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_ypre_stepoffset_value_changed       (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_ypre_randnumber_value_changed       (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);
