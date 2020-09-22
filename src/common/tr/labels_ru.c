#include "labels.h"

const char *const labels_ru[LABEL_END] = {
    [LABEL_SUNDAY]          = "Воскресенье",
    [LABEL_MONDAY]          = "Понедельник",
    [LABEL_TUESDAY]         = "Вторник",
    [LABEL_WEDNESDAY]       = "Среда",
    [LABEL_THURSDAY]        = "Четверг",
    [LABEL_FRIDAY]          = "Пятница",
    [LABEL_SATURDAY]        = "Cуббота",

    [LABEL_JANUARY]         = "января",
    [LABEL_FEBRUARY]        = "февраля",
    [LABEL_MARCH]           = "марта",
    [LABEL_APRIL]           = "апреля",
    [LABEL_MAY]             = "мая",
    [LABEL_JUNE]            = "июня",
    [LABEL_JULY]            = "июля",
    [LABEL_AUGUST]          = "августа",
    [LABEL_SEPTEMBER]       = "сентября",
    [LABEL_OCTOBER]         = "октября",
    [LABEL_NOVEMBER]        = "ноября",
    [LABEL_DECEMBER]        = "декабря",

    [LABEL_VOLUME]          = "Громкость",
    [LABEL_BASS]            = "Тембр НЧ",
    [LABEL_MIDDLE]          = "Тембр СЧ",
    [LABEL_TREBLE]          = "Тембр ВЧ",
    [LABEL_FRONTREAR]       = "Фронт/тыл",
    [LABEL_BALANCE]         = "Баланс",
    [LABEL_CENTER]          = "Центр",
    [LABEL_SUBWOOFER]       = "Сабвуфер",
    [LABEL_PREAMP]          = "Усиление",

    [LABEL_IN_TUNER]        = "Тюнер",
    [LABEL_IN_PC]           = "Компьютер",
    [LABEL_IN_TV]           = "Телевизор",
    [LABEL_IN_BLUETOOTH]    = "Bluetooth",
    [LABEL_IN_DVD]          = "DVD",
    [LABEL_IN_USB]          = "USB",
    [LABEL_IN_MICROPHONE]   = "Микрофон",
    [LABEL_IN_GUITAR]       = "Гитара",
    [LABEL_IN_TURNTABLES]   = "Вертушки",
    [LABEL_IN_SDCARD]       = "SD карта",
    [LABEL_IN_PROJECTOR]    = "Проектор",
    [LABEL_IN_SATELLITE]    = "Спутник",
    [LABEL_IN_MIXER]        = "Микшер",
    [LABEL_IN_KARADIO]      = "KaRadio",

    [LABEL_BOOL_OFF]        = "ВЫКЛ",
    [LABEL_BOOL_ON]         = "ВКЛ",

    [LABEL_SETUP_MAIN]      = "Настройки",
    [LABEL_SETUP_TIME]      = "Время",
    [LABEL_SETUP_DATE]      = "Дата",
    [LABEL_SETUP_ALARM]     = "Будильник",
    [LABEL_SETUP_REMOTE]    = "Пульт ДУ",

    [LABEL_ALARM_DAY_OFF]       = "отключен",
    [LABEL_ALARM_DAY_WEEKDAYS]  = "будни",
    [LABEL_ALARM_DAY_ALL_DAYS]  = "все дни",

    // NOTE: Keep in sync with cmd.h define list
    [LABEL_RC_STBY_SWITCH]  = "Режим ожидания",
    [LABEL_RC_VOL_UP]       = "Громкость +",
    [LABEL_RC_VOL_DOWN]     = "Громкость -",
    [LABEL_RC_MUTE]         = "Приглушение",

    [LABEL_RC_MENU]         = "Меню",
    [LABEL_RC_CHAN_PREV]    = "Предыдущий канал",
    [LABEL_RC_CHAN_NEXT]    = "Следующий канал",
    [LABEL_RC_DIG_0]        = "Цифра 0",
    [LABEL_RC_DIG_1]        = "Цифра 1",
    [LABEL_RC_DIG_2]        = "Цифра 2",
    [LABEL_RC_DIG_3]        = "Цифра 3",
    [LABEL_RC_DIG_4]        = "Цифра 4",
    [LABEL_RC_DIG_5]        = "Цифра 5",
    [LABEL_RC_DIG_6]        = "Цифра 6",
    [LABEL_RC_DIG_7]        = "Цифра 7",
    [LABEL_RC_DIG_8]        = "Цифра 8",
    [LABEL_RC_DIG_9]        = "Цифра 9",
    [LABEL_RC_IN_PREV]      = "Предыдущий вход",
    [LABEL_RC_IN_NEXT]      = "Следующий вход",
    [LABEL_RC_NAV_OK]       = "OK",
    [LABEL_RC_NAV_BACK]     = "Назад",
    [LABEL_RC_NAV_RIGHT]    = "Вправо",
    [LABEL_RC_NAV_UP]       = "Вверх",
    [LABEL_RC_NAV_LEFT]     = "Влево",
    [LABEL_RC_NAV_DOWN]     = "Вниз",
    [LABEL_RC_BASS_UP]      = "НЧ +",
    [LABEL_RC_BASS_DOWN]    = "НЧ -",
    [LABEL_RC_MIDDLE_UP]    = "СЧ +",
    [LABEL_RC_MIDDLE_DOWN]  = "СЧ -",
    [LABEL_RC_TREBLE_UP]    = "ВЧ +",
    [LABEL_RC_TREBLE_DOWN]  = "ВЧ -",
    [LABEL_RC_LOUDNESS]     = "Тонкоррекция",
    [LABEL_RC_TIME]         = "Час",
    [LABEL_RC_STOP]         = "Стоп",
    [LABEL_RC_PLAY_PAUSE]   = "Играть/Пауза",
    [LABEL_RC_REW]          = "Шаг назад",
    [LABEL_RC_FWD]          = "Шаг вперёд",
    [LABEL_RC_TIMER]        = "Таймер",
    [LABEL_RC_SP_MODE]      = "Режим спектра",
    [LABEL_RC_SCR_DEF]      = "Главный экран",
};
