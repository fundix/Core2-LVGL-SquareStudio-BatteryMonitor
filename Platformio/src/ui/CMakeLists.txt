SET(SOURCES ui_Screen1.c
    ui.c
    ui_comp_hook.c
    ui_helpers.c
    fonts/ui_font_H1.c
    fonts/ui_font_H3.c)

add_library(ui ${SOURCES})
