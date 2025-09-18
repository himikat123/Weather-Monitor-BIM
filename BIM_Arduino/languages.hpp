class Lang {
    private:
        /* English, German, Russian, Polish, Ukrainian, Bulgarian, Spanish */
        String _weekdayShortName[7][7] = {
            {"Su", "So", "Вс", "Nd", "Нд", "Не", "Do"},
            {"Mo", "Mo", "Пн", "Pn", "Пн", "По", "Lu"},
            {"Tu", "Di", "Вт", "Wt", "Вт", "Вт", "Ma"},
            {"We", "Mi", "Ср", "Śr", "Ср", "Ср", "Mi"},
            {"Th", "Do", "Чт", "Cz", "Чт", "Че", "Ju"},
            {"Fr", "Fr", "Пт", "Pi", "Пт", "Пе", "Vi"},
            {"Sa", "Sa", "Сб", "So", "Сб", "Съ", "Sá"}
        };

        String _monthShortName[12][7] = {
            {"Jan", "Jan", "янв", "sty", "січ", "яну", "ene"},
            {"Feb", "Feb", "фев", "lut", "лют", "фев", "feb"},
            {"Mar", "Mär", "мар", "mar", "бер", "мар", "mar"},
            {"Apr", "Apr", "апр", "kwi", "кві", "апр", "abr"},
            {"May", "Mai", "мая", "maj", "тра", "май", "may"},
            {"Jun", "Jun", "июн", "cze", "чер", "юни", "jun"},
            {"Jul", "Jul", "июл", "lip", "лип", "юли", "jul"},
            {"Aug", "Aug", "авг", "sie", "сер", "авг", "ago"},
            {"Sep", "Sep", "сен", "wrz", "вер", "сеп", "sep"},
            {"Oct", "Okt", "окт", "paź", "жов", "окт", "oct"},
            {"Nov", "Nov", "ноя", "lis", "лис", "ное", "nov"},
            {"Dec", "Dez", "дек", "gru", "гру", "дек", "dic"}
        };

        String _comfort[9][7] = {
            {"Comfortable", "Gemütlich", "Комфортно", "Przyjemny", "Комфортно", "Комфортно", "Clima agradable"},
            {"Too hot", "Zu heiß", "Жарко", "Gorąco", "Жарко", "Горещо", "Muy caliente"},
            {"Too cold", "Zu kalt", "Холодно", "Zimno", "Холодно", "Студено", "Muy frío"},
            {"Too humid", "Zu feucht", "Слишком влажно", "Zbyt wilgotno", "Занадто волого", "Твърде влажно", "Muy húmedo"},
            {"Too dry", "Zu trocken", "Слишком сухо", "Zbyt sucho", "Занадто сухо", "Твърде сухо", "Muy seco"},
            {"Hot and humid", "Heiß und feucht", "Жарко и влажно", "Gorąco i wilgotno", "Жарко та волого", "Горещо и влажно", "Caliente y húmedo"},
            {"Hot and dry", "Heiß und trocken", "Жарко и сухо", "Gorąco i sucho", "Жарко та сухо", "Горещо и сухо", "Caliente y seco"},
            {"Cold and humid", "Kalt und feucht", "Холодно и влажно", "Zimno i wilgotno", "Холодно та волого", "Студено и влажно", "Frío y húmedo"},
            {"Cold and dry", "Kalt und trocken", "Холодно и сухо", "Zimno i sucho", "Холодно та сухо", "Студено и сухо", "Frío y seco"}
        };

        String _weatherDescription[16][7] = {
            {"Clear sky", "Klarer Himmel", "Ясно", "Czyste Niebo", "Ясно", "Ясно", "Despejado"},
            {"Mainly clear", "Überwiegend klar", "Преимущественно ясно", "Głównie jasne", "Переважно ясно", "Предимно ясно", "Mayormente despejado"},
            {"Partly cloudy", "Teilweise bewölkt", "Переменная облачность", "Częściowe zachmurzenie", "Мінлива хмарність", "Предимно облачно", "Parcialmente nublado"},
            {"Overcast", "Bedeckt", "Пасмурно", "Pochmurny", "Хмарно", "Облачно", "Nublado"},
            {"Fog", "Nebel", "Туман", "Mgła", "Туман", "Мъгла", "Neblina"},
            {"Depositing rime fog", "Ablagerung von Raureifnebel", "Туман с инеем", "Deponująca się mgła szronowa", "Туман з інеєм", "Мъгла със скреж", "Escarcha"},
            {"Drizzle", "Nieselregen", "Морось", "Mżawka", "Мряка", "Дъжд", "Llovizna"},
            {"Freezing drizzle", "Gefrierender Nieselregen", "Изморозь", "Marznąca mżawka", "Ізморозь", "Скреж", "Llovizna helada"},
            {"Rain", "Regen", "Дождь", "Deszcz", "Дощ", "Дъжд", "Lluvia"},
            {"Freezing rain", "Gefrierender Regen", "Ледяной дождь", "Marznący deszcz", "Крижаний дощ", "Леден дъжд", "Lluvia helada"},
            {"Snowfall", "Schneefall", "Снегопад", "Opady śniegu", "Снігопад", "Снеговалеж", "Nevada"},
            {"Snow grains", "Schneekörner", "Снежные зерна", "Ziarna śniegu", "Снігові зерна", "Скреж", "Copos de nieve"},
            {"Rain showers", "Regenschauer", "Ливень", "Przelotne opady deszczu", "Злива", "Ръмеж", "Chubascos"},
            {"Snow showers", "Schneeschauer", "Сильный снегопад", "Opady śniegu", "Сильний снігопад", "Обилен снеговалеж", "Tormenta de nieve"},
            {"Thunderstorm", "Gewitter", "Гроза", "Burza z piorunami", "Гроза", "Буря", "Tormenta eléctrica"},
            {"Thunderstorm with hail", "Gewitter mit Hagel", "Гроза с градом", "Burza z gradem", "Гроза з градом", "Буря с градушка", "Tormenta eléctrica con granizo"}
        };

        String _mm[7] = {"mm", "mm", "мм", "mm", "мм", "мм", "mm"};
        String _hpa[7] = {"hPa", "hPa", "гПа", "hPa", "гПа", "гПа", "hPa"};
        String _ms[7] = {"m/s", "m/s", "м/с", "m/s", "м/с", "м/с", "m/s"};
        String _gpm[7] = {"g/m³", "g/m³", "г/м³", "g/m³", "г/м³", "г/м³", "g/m³"};
        String _v[7] = {"V", "V", " В", "V", " В", " В", "V"};
    
        uint8_t _lang() {
            if(config.lang() == "de") return 1;
            if(config.lang() == "ru") return 2;
            if(config.lang() == "pl") return 3;
            if(config.lang() == "ua") return 4;
            if(config.lang() == "bg") return 5;
            if(config.lang() == "es") return 6;
            return 0;
        }


    public:
        String weekdayShortName(uint8_t num) {
            if(num < 1 || num > 7) return "???";
            return _weekdayShortName[num - 1][_lang()];
        }

        String monthShortName(uint8_t num) {
            if(num < 1 || num > 12) return "???";
            return _monthShortName[num - 1][_lang()];
        }

        String mm() {
            return _mm[_lang()];
        }

        String hpa() {
            return _hpa[_lang()];
        }

        String ms() {
            return _ms[_lang()];
        }

        String gpm() {
            return _gpm[_lang()];
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
