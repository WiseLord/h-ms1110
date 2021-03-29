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
    [LABEL_BALANCE]         = "Баланс",
    [LABEL_FRONTREAR]       = "Фронт/тыл",
    [LABEL_CENTER]          = "Центр",
    [LABEL_SUBWOOFER]       = "Сабвуфер",
    [LABEL_PREAMP]          = "Усиление",
    [LABEL_LOUDNESS]        = "Тонкоррекция",

    [LABEL_BASS_FREQ]       = "Частота НЧ",
    [LABEL_BASS_QUAL]       = "Добротность НЧ",
    [LABEL_MIDDLE_KFREQ]    = "Частота СЧ",
    [LABEL_MIDDLE_QUAL]     = "Добротность СЧ",
    [LABEL_TREBLE_KFREQ]    = "Частота ВЧ",
    [LABEL_SUB_CUT_FREQ]    = "Полоса саба",
    [LABEL_LOUD_PEAK_FREQ]  = "Частота тонкорр.",

    [LABEL_IN_TUNER]        = "Тюнер",
    [LABEL_IN_MPD]          = "MPD",
    [LABEL_IN_AUX1]         = "AUX1",
    [LABEL_IN_AUX2]         = "AUX2",
    [LABEL_IN_SPDIF]        = "SPDIF",

    [LABEL_BOOL_OFF]        = "ВЫКЛ",
    [LABEL_BOOL_ON]         = "ВКЛ",

    [LABEL_DB]              = "дБ",
    [LABEL_HZ]              = "Гц",
    [LABEL_K]               = "к",

    [LABEL_SETUP_MAIN]      = "Настройки",
    [LABEL_SETUP_TIME]      = "Время",
    [LABEL_SETUP_DATE]      = "Дата",
    [LABEL_SETUP_ALARM]     = "Будильник",
    [LABEL_SETUP_REMOTE]    = "Пульт ДУ",

    [LABEL_ALARM_DAY_OFF]       = "отключен",
    [LABEL_ALARM_DAY_WEEKDAYS]  = "будни",
    [LABEL_ALARM_DAY_ALL_DAYS]  = "все дни",

    [LABEL_MPD_WAIT]        = "Ожидание MPD...",
};
