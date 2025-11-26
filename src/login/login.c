// 2025-11-20 ÀÌ¿¬Áö: ·Î±×ÀÎ ÆäÀÌÁö Á¦ÀÛ

#include "login.h"
#include "windows.h"

/**
 * @brief ±âº»ÀûÀÎ ·Î±×ÀÎ UI¸¦ ¶ç¿öÁØ´Ù.(¸Å Æ½¸¶´Ù ½ÇÇàµÇ¾î¾ß ÇÔ)
 * @param select ÇöÀç Ä¿¼­°¡ ¿Ã¶ó¿Í ÀÖ´Â Ç×¸ñÀÌ´Ù.(·Î±×ÀÎ/ ÀÌ¸§ ¼öÁ¤)
 * @param id_buffer ÇöÀç±îÁö ÀÔ·ÂÇÑ idÀÇ ¹®ÀÚ¿­ÀÌ´Ù.
 * @return ¼º°ø ¿©ºÎ¸¦ StatusCodeEnumÀ¸·Î ¹ÝÈ¯ÇÑ´Ù. Ç×»ó SUCCESS¸¦ ¹ÝÈ¯ÇÑ´Ù.
 */
StatusCodeEnum draw_ui(SelectEnum select, const char* id_buffer) {
    system("cls");
    
    print_center("=== LOGIN ===", 13, 10);
    
    print_center("User ID: ", 33, 12);
    if (select == ID_FIELD) {
        printf("%s> [%-20s]%s", UI_REVERSE, id_buffer, UI_RESET);
        goto_ansi(START_X + (UI_WIDTH - 33) / 2 + (int)strlen(id_buffer), 12);
    } else {
        printf("  [%-20s]", id_buffer);
    }

    goto_ansi(START_X + (UI_WIDTH - 16)/2, 15);
    if (select == LOGIN_BUTTON) {
        printf("%s[ >> LOGIN << ]%s", UI_REVERSE, UI_RESET);
    } else {
        printf("[    LOGIN    ]");
    }
    fflush(stdout);

    return SUCCESS;
}

/**
 * @brief ë¡œê·¸?¸ UIë¥? ?„?š°ê³? ë¡œê·¸?¸ ? •ë³´ë?? ?ž…? ¥ë°›ëŠ” ?•¨?ˆ˜?´?‹¤.
 * @param id_buffer ?ž…? ¥ë°›ì?? idë¥? ????ž¥?•˜ê³?, ë°˜í™˜?•˜?Š” ë¬¸ìž?—´?´?‹¤. ?•´?‹¹ ?¬?¸?„°?— ?•„?´?””ê°? ????ž¥?œ?‹¤.
 * @param max_len ?ž…? ¥ë°›ì„ id?˜ ìµœë?? ê¸¸ì´?´?‹¤.
 * @return ?„±ê³? ?—¬ë¶?ë¥? StatusCodeEnum?œ¼ë¡? ë°˜í™˜?•œ?‹¤. ?•­?ƒ SUCCESSë¥? ë°˜í™˜?•œ?‹¤.
 */
StatusCodeEnum show_login_screen(char *id_buffer, int max_len) {
    SelectEnum current = ID_FIELD;
    int id_index = 0;
    id_buffer[0] = '\0';
    
    KeyEnum ch;
    while (1) {
        StatusCodeEnum draw_ui_s = draw_ui(current, id_buffer);

        ch = _getch(); 

        switch (ch) {
            case PRE_INPUT1:
            case PRE_INPUT2:
                ch = _getch();
                if (ch == UP_ARROW || ch == LEFT_ARROW) { // Up or Left
                    current = (current == ID_FIELD) ? LOGIN_BUTTON : ID_FIELD;
                } else if (ch == DOWN_ARROW || ch == RIGHT_ARROW) { // Down or Right
                    current = (current == ID_FIELD) ? LOGIN_BUTTON : ID_FIELD;
                }
                break;

            case ENTER:
                if (current == LOGIN_BUTTON) return SUCCESS;
                break;
            
            case BACKSPACE:
                if (current == ID_FIELD && id_index > 0) {
                    id_index--;
                    id_buffer[id_index] = '\0';
                }
                break;

            default:
                if (current == ID_FIELD && id_index < max_len - 1 && ch >= '0' && ch <= '9') {
                    id_buffer[id_index] = (char)ch;
                    id_index++;
                    id_buffer[id_index] = '\0';
                }
                break;
        }
    }

    return SUCCESS;
}

/**
 * @brief ë¡œê·¸?¸ UIë¥? ?„?š°ê³? ë¡œê·¸?¸ ? •ë³´ë?? ?ž…? ¥ë°›ëŠ” ?•¨?ˆ˜?´?‹¤. show_login_screen?— ?•„?š”?•œ ê¸°ëŠ¥?“¤?„ ? •ë¦¬í•´?†“??? ?•¨?ˆ˜?´?‹¤.
 * @param user_id ?ž…? ¥ë°›ì?? idë¥? ????ž¥?•˜ê³?, ë°˜í™˜?•˜?Š” ë¬¸ìž?—´?´?‹¤. ?•´?‹¹ ?¬?¸?„°?— ?•„?´?””ê°? ????ž¥?œ?‹¤.
 * @return ?„±ê³? ?—¬ë¶?ë¥? StatusCodeEnum?œ¼ë¡? ë°˜í™˜?•œ?‹¤. ?•­?ƒ SUCCESSë¥? ë°˜í™˜?•œ?‹¤.
 */
StatusCodeEnum login(int *user_id) {
    system("cls");
    char id_str[ID_LENGTH];
    StatusCodeEnum s_show_login_screen = show_login_screen(id_str, ID_LENGTH);
    *user_id = atoi(id_str);
    return SUCCESS;
}