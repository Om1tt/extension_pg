#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "utils/memutils.h"
#include "postgres_fe.h"
#include "lib/stringinfo.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

PG_FUNCTION_INFO_V1(convert_temperature);

Datum convert_temperature(PG_FUNCTION_ARGS);

// Перечисление, представляющее три возможные шкалы температуры
typedef enum {
    CELSIUS,
    KELVIN,
    FAHRENHEIT
} TempScale;

// Функция для преобразования текстового представления шкалы температуры в соответствующее значение TempScale
static TempScale get_scale_from_text(text *scale_text) {
    
    char *scale_str = VARDATA(scale_text);
    int len = VARSIZE(scale_text) - VARHDRSZ;

    if (strncmp(scale_str, "cels", len) == 0) {
        return CELSIUS;
    } else if (strncmp(scale_str, "kelv", len) == 0) {
        return KELVIN;
    } else if (strncmp(scale_str, "far", len) == 0) {
        return FAHRENHEIT;
    } else {
        ereport(ERROR,
            (errmsg("Unknown temperature scale: %.*s", len, scale_str)));
    }
    return CELSIUS;
}

// Функция конвертации температуры
Datum
convert_temperature(PG_FUNCTION_ARGS)
{
    // Получаем значения аргументов функции
    float8 temp_value = PG_GETARG_FLOAT8(0);
    text *from_scale_text = PG_GETARG_TEXT_P(1);
    text *to_scale_text = PG_GETARG_TEXT_P(2);

    // Преобразуем текстовое представление исходной шкалы температуры в тип TempScale
    TempScale from_scale = get_scale_from_text(from_scale_text);
    TempScale to_scale = get_scale_from_text(to_scale_text);

    // Переменная результата
    float8 result = temp_value;

    // Преобразуем исходную температуру в градусы Цельсия
    switch (from_scale) {
        case CELSIUS:
            break; // Уже в Цельсия
        case KELVIN:
            result = temp_value - 273.15;
            break;
        case FAHRENHEIT:
            result = (temp_value - 32) * 5.0 / 9.0;
            break;
    }

    // Преобразуем результат из Цельсия в целевую шкалу температуры
    switch (to_scale) {
        case CELSIUS:
            break; // Уже в цельсия
        case KELVIN:
            result = result + 273.15;
            break;
        case FAHRENHEIT:
            result = result * 9.0 / 5.0 + 32;
            break;
    }

    PG_RETURN_FLOAT8(result);

    /*
        Прямое преобразование температуры между шкалами. 
        Мне больше понравился вариант с промежуточным Цельсия, тк меньше формул и упрощает добавление новых шкал (Ранкин, Делисль и т.д.)
        
        if (from_scale == CELSIUS && to_scale == KELVIN) {
            result = temp_value + 273.15;
        } else if (from_scale == CELSIUS && to_scale == FAHRENHEIT) {
            result = temp_value * 9.0 / 5.0 + 32;
        } else if (from_scale == KELVIN && to_scale == CELSIUS) {
            result = temp_value - 273.15;
        } else if (from_scale == KELVIN && to_scale == FAHRENHEIT) {
            result = (temp_value - 273.15) * 9.0 / 5.0 + 32;
        } else if (from_scale == FAHRENHEIT && to_scale == CELSIUS) {
            result = (temp_value - 32) * 5.0 / 9.0;
        } else if (from_scale == FAHRENHEIT && to_scale == KELVIN) {
            result = (temp_value - 32) * 5.0 / 9.0 + 273.15;
        } 
    */
}