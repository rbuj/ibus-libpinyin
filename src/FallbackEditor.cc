#include "FallbackEditor.h"
#include "HalfFullConverter.h"

namespace PY {

#define CMSHM_MASK              \
        (IBUS_CONTROL_MASK |    \
         IBUS_MOD1_MASK |       \
         IBUS_SUPER_MASK |      \
         IBUS_HYPER_MASK |      \
         IBUS_META_MASK)

#define CMSHM_FILTER(modifiers)  \
    (modifiers & (CMSHM_MASK))

inline gboolean
FallbackEditor::processPunct (guint keyval, guint keycode, guint modifiers)
{
    guint cmshm_modifiers = CMSHM_FILTER (modifiers);

    if (G_UNLIKELY (keyval == IBUS_period && cmshm_modifiers == IBUS_CONTROL_MASK)) {
        m_props.toggleModeFullPunct ();
        return TRUE;
    }

    /* check ctrl, alt, hyper, supper masks */
    if (cmshm_modifiers != 0)
        return FALSE;

    /* English mode */
    if (G_UNLIKELY (!m_props.modeChinese ())) {
        if (G_UNLIKELY (m_props.modeFull ()))
            commit (HalfFullConverter::toFull (keyval));
        else
            commit (keyval);
        return TRUE;
    }
    else {
        /* Chinese mode */
        if (m_props.modeFullPunct ()) {
            switch (keyval) {
            case '`':
                commit ("·"); return TRUE;
            case '~':
                commit ("～"); return TRUE;
            case '!':
                commit ("！"); return TRUE;
            // case '@':
            // case '#':
            case '$':
                commit ("￥"); return TRUE;
            // case '%':
            case '^':
                commit ("……"); return TRUE;
            // case '&':
            // case '*':
            case '(':
                commit ("（"); return TRUE;
            case ')':
                commit ("）"); return TRUE;
            // case '-':
            case '_':
                commit ("——"); return TRUE;
            // case '=':
            // case '+':
            case '[':
                commit ("【"); return TRUE;
            case ']':
                commit ("】"); return TRUE;
            case '{':
                commit ("『"); return TRUE;
            case '}':
                commit ("』"); return TRUE;
            case '\\':
                commit ("、"); return TRUE;
            // case '|':
            case ';':
                commit ("；"); return TRUE;
            case ':':
                commit ("："); return TRUE;
            case '\'':
                commit (m_quote ? "‘" : "’");
                m_quote = !m_quote;
                return TRUE;
            case '"':
                commit (m_double_quote ? "“" : "”");
                m_double_quote = !m_double_quote;
                return TRUE;
            case ',':
                commit ("，"); return TRUE;
            case '.':
                if (m_prev_commited_char >= '0' && m_prev_commited_char <= '9')
                    commit (keyval);
                else
                    commit ("。");
                return TRUE;
            case '<':
                commit ("《"); return TRUE;
            case '>':
                commit ("》"); return TRUE;
            // case '/':
            case '?':
                commit ("？"); return TRUE;
            }
        }
        commit (m_props.modeFull () ? HalfFullConverter::toFull (keyval) : keyval);
    }
    return TRUE;
}

gboolean
FallbackEditor::processKeyEvent (guint keyval, guint keycode, guint modifiers)
{
    gboolean retval = FALSE;

    modifiers &= (IBUS_SHIFT_MASK |
                  IBUS_CONTROL_MASK |
                  IBUS_MOD1_MASK |
                  IBUS_SUPER_MASK |
                  IBUS_HYPER_MASK |
                  IBUS_META_MASK);

    switch (keyval) {
        /* letters */
        case IBUS_a ... IBUS_z:
        case IBUS_A ... IBUS_Z:
        /* numbers */
        case IBUS_KP_0 ... IBUS_KP_9:
            keyval = keyval + IBUS_0 - IBUS_KP_0;
        case IBUS_0 ... IBUS_9:
            if (modifiers == 0) {
                commit (m_props.modeFull () ? HalfFullConverter::toFull (keyval) : (gchar) keyval);
                retval = TRUE;
            }
            break;
        /* punct */
        case IBUS_exclam ... IBUS_slash:
        case IBUS_colon ... IBUS_at:
        case IBUS_bracketleft ... IBUS_quoteleft:
        case IBUS_braceleft ... IBUS_asciitilde:
            retval = processPunct (keyval, keycode, modifiers);
            break;
        case IBUS_KP_Equal:
            retval = processPunct ('=', keycode, modifiers);
            break;
        case IBUS_KP_Multiply:
            retval = processPunct ('*', keycode, modifiers);
            break;
        case IBUS_KP_Add:
            retval = processPunct ('+', keycode, modifiers);
            break;
        #if 0
        case IBUS_KP_Separator:
            retval = processPunct (IBUS_separator, keycode, modifiers);
            break;
        #endif
        case IBUS_KP_Subtract:
            retval = processPunct ('-', keycode, modifiers);
            break;
        case IBUS_KP_Decimal:
            retval = processPunct ('.', keycode, modifiers);
            break;
        case IBUS_KP_Divide:
            retval = processPunct ('/', keycode, modifiers);
            break;
        /* space */
        case IBUS_KP_Space:
            keyval = IBUS_space;
        case IBUS_space:
            if (modifiers == 0) {
                commit (m_props.modeFull () ? "　" : " ");
                retval = TRUE;
            }
            break;
        /* others */
        default:
            break;
    }
    return retval;
}

void
FallbackEditor::reset (void) {
    m_quote = TRUE;
    m_double_quote = TRUE;
}


};