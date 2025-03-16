class Lang {
    private:
        /* English, German, Russian, Polish, Ukrainian, Bulgarian */
        String _weekdayShortName[7][6] = {
            {"Su", "So", "Вс", "Nd", "Нд", "Не"},
            {"Mo", "Mo", "Пн", "Pn", "Пн", "По"},
            {"Tu", "Di", "Вт", "Wt", "Вт", "Вт"},
            {"We", "Mi", "Ср", "Śr", "Ср", "Ср"},
            {"Th", "Do", "Чт", "Cz", "Чт", "Че"},
            {"Fr", "Fr", "Пт", "Pi", "Пт", "Пе"},
            {"Sa", "Sa", "Сб", "So", "Сб", "Съ"}
        };

        String _comfort[9][6] = {
            {"Comfortable", "Gemütlich", "Комфортно", "Przyjemny", "Комфортно", "Комфортно"},
            {"Too hot", "Zu heiß", "Жарко", "Gorąco", "Жарко", "Горещо"},
            {"Too cold", "Zu kalt", "Холодно", "Zimno", "Холодно", "Студено"},
            {"Too humid", "Zu feucht", "Слишком влажно", "Zbyt wilgotno", "Занадто волого", "Твърде влажно"},
            {"Too dry", "Zu trocken", "Слишком сухо", "Zbyt sucho", "Занадто сухо", "Твърде сухо"},
            {"Hot and humid", "Heiß und feucht", "Жарко и влажно", "Gorąco i wilgotno", "Жарко та волого", "Горещо и влажно"},
            {"Hot and dry", "Heiß und trocken", "Жарко и сухо", "Gorąco i sucho", "Жарко та сухо", "Горещо и сухо"},
            {"Cold and humid", "Kalt und feucht", "Холодно и влажно", "Zimno i wilgotno", "Холодно та волого", "Студено и влажно"},
            {"Cold and dry", "Kalt und trocken", "Холодно и сухо", "Zimno i sucho", "Холодно та сухо", "Студено и сухо"}
        };

        String _weatherDescription[16][6] = {
            {"Clear sky", "Klarer Himmel", "Ясно", "Czyste Niebo", "Ясно", "Ясно"},
            {"Mainly clear", "Überwiegend klar", "Преимущественно ясно", "Głównie jasne", "Переважно ясно", "Предимно ясно"},
            {"Partly cloudy", "Teilweise bewölkt", "Переменная облачность", "Częściowe zachmurzenie", "Мінлива хмарність", "Предимно облачно"},
            {"Overcast", "Bedeckt", "Пасмурно", "Pochmurny", "Хмарно", "Облачно"},
            {"Fog", "Nebel", "Туман", "Mgła", "Туман", "Мъгла"},
            {"Depositing rime fog", "Ablagerung von Raureifnebel", "Туман с инеем", "Deponująca się mgła szronowa", "Туман з інеєм", "Мъгла със скреж"},
            {"Drizzle", "Nieselregen", "Морось", "Mżawka", "Мряка", "Дъжд"},
            {"Freezing drizzle", "Gefrierender Nieselregen", "Изморозь", "Marznąca mżawka", "Ізморозь", "Скреж"},
            {"Rain", "Regen", "Дождь", "Deszcz", "Дощ", "Дъжд"},
            {"Freezing rain", "Gefrierender Regen", "Ледяной дождь", "Marznący deszcz", "Крижаний дощ", "Леден дъжд"},
            {"Snowfall", "Schneefall", "Снегопад", "Opady śniegu", "Снігопад", "Снеговалеж"},
            {"Snow grains", "Schneekörner", "Снежные зерна", "Ziarna śniegu", "Снігові зерна", "Скреж"},
            {"Rain showers", "Regenschauer", "Ливень", "Przelotne opady deszczu", "Злива", "Ръмеж"},
            {"Snow showers", "Schneeschauer", "Сильный снегопад", "Opady śniegu", "Сильний снігопад", "Обилен снеговалеж"},
            {"Thunderstorm", "Gewitter", "Гроза", "Burza z piorunami", "Гроза", "Буря"},
            {"Thunderstorm with hail", "Gewitter mit Hagel", "Гроза с градом", "Burza z gradem", "Гроза з градом", "Буря с градушка"}
        };

        String _mm[6] = {"mm", "mm", "мм", "mm", "мм", "мм"};
        String _ms[6] = {"m/s", "m/s", "м/с", "m/s", "м/с", "м/с"};
        String _v[6] = {"V", "V", " В", "V", " В", " В"};
    
        uint8_t _lang() {
            if(config.lang() == "de") return 1;
            if(config.lang() == "ru") return 2;
            if(config.lang() == "pl") return 3;
            if(config.lang() == "ua") return 4;
            if(config.lang() == "bg") return 5;
            return 0;
        }


    public:
        String weekdayShortName(uint8_t num) {
            if(num < 1 || num > 7) return "???";
            return _weekdayShortName[num - 1][_lang()];
        }

        String mm() {
            return _mm[_lang()];
        }

        String ms() {
            return _ms[_lang()];
        }

        String v() {
            return _v[_lang()];
        }

        String comfort(unsigned int level) {
            if(level < 1 || level > 9) return "";
            return _comfort[level - 1][_lang()];
        }

        String weatherDescription(unsigned int level) {
            if(level > 15) return "???";
            return _weatherDescription[level][_lang()];
        }
};
