struct saved_text{
  char* saved;
  char* not_saved;
};

struct saved_text saved[8]={
  "Saved",
  "Did not save",
    
  "Сохранено",
  "Не сохранено",
    
  "Salvat",
  "Nu este salvat",
    
  "Gerettet",
  "Nicht gespeichert",

  "Išsaugota",
  "Nepasaugotas",
  
  "Salvestatud",
  "Ei salvestatud",

  "Збережено",
  "Не збережено",

  "Yadda saxlanıldı",
  "Yadda saxlanılmadı"
};

struct status_text{
  char* connecting_to;
  char* unable_to_connect_to;
  char* connected_to;
  char* network;
  char* not_found;
  char* unable_connect_sensor;
  char* connecting_sensor;
};

struct status_text status_lng[8]={
  "Connecting to",
  "Unable to connect to the network",
  "Connected to",
  "Network ",
  " not found",
  "Unable to connect to the Sensor",
  "Connecting to the Sensor",
  
  "Соединение с",
  "Не удается подключиться к сети",
  "Подключено к",
  "Сеть ",
  " не найдена",
  "Не удается подключиться к Датчику",
  "Подключение к Датчику",
  
  "Conectare cu",
  "Nu se poate conecta la rețea",
  "Conectat la",
  "Rețeaua ",
  " nu a fost găsită",
  "Nu se poate conecta la Senzor",
  "Conectarea la Senzor",
  
  "Verbinden mit",
  "keine Verbindung zum Netzwerk",
  "Angeschlossen",
  "Netzwerk ",
  " nicht gefunden",
  "Verbindung zum Sensor nicht möglich",
  "Verbindung zum Sensor",

  "Prisijungiama prie",
  "Nepavyko prisijungti prie tinklo",
  "Sujungta su",
  "Tinklas ",
  " nerastas",
  "Nepavyko prisijungti prie jutiklio",
  "Prijungimas prie Jutiklio",

  "Ühend",
  "Võrku ei saa ühendust luua",
  "Ühendatud",
  "Võrk ",
  " ei leitud",
  "Anduriga ei saa ühendust luua",
  "Anduriga ühendamine",

  "З’єднання з",
  "Не вдається підключитись до мережі",
  "Підключено до",
  "Мережу ",
  " не знайдено",
  "Неможливо підключитися до Датчика",
  "Підключення до Датчика",

  "Bağlantı",
  "Şəbəkəyə qoşula bilmir",
  "Qoşuldu",
  "",
  "Şəbəkə tapılmadı",
  "Sensora qoşulmaq mümkün deyil",
  "Sensora qoşulur"
};

struct settings_text{
  char* str1;
  char* str2;
  char* str3;
  char* str4;
  char* str5;
  char* str6;
  char* str7;
};

struct settings_text settings_lng[8]={
  "To enter setup",
  "connect your laptop",
  "tablet or smartphone to",
  " ",
  "password",
  "Open your browser and enter",
  "address http://192.168.4.1",

  "Чтоб войти в настройки",
  "подключите ваш ноутбук,",
  "планшет или смартфон к",
  "сети",
  "пароль",
  "Откройте браузер и введите",
  "адрес http://192.168.4.1",

  "Pentru a intra în setări",
  "conectați laptopul,",
  "tableta sau smartphone",
  "la rețeaua",
  "parola",
  "și introduceți adresa",
  "http://192.168.4.1 în browser",

  "So geben sie setup",
  "schließen sie Ihren laptop",
  "tablet oder smartphone",
  "network",
  "passwort",
  "Öffnen sie Ihren browser und geben",
  "Sie adresse http://192.168.4.1",

  "Norint patekti į nustatymus",
  "prijunkite savo nešiojamą kompiuterį",
  "arba išmanujį įrenginį prie",
  "tinklo",
  "slaptažodis",
  "Atidarykite naršyklę",
  "ir įveskite adresą http://192.168.4.1",

  "Seadete sisestamiseks",
  "ühendage oma sülearvuti",
  "tahvelarvuti või nutitelefoni",
  "võrgu",
  "parooli",
  "Avage veebibrauser ja",
  "sisestage http://192.168.4.1 aadress",

  "Для того щоб перейти",
  "до налаштувань підключіть",
  "ваш ноутбук, планшет чи смартфон",
  "до мережі",
  "пароль",
  "Відкрийте браузер та наберіть",
  "адресу http://192.168.4.1",

  "Parametrləri daxil etmək",
  "üçün laptopunuzu,",
  "tabletinizi və ya smartfonu",
  "",
  "şəbəkəsinə qoşun, şifrə",
  "Brauzeri açın və http://192.168.4.1",
  "ünvanını daxil edin"
};

char* Month[12][8]={
  {" January ",  " Января ",  " Ianuarie ",  " Januar ",   " Sausis ",   " Jaanuar ",  " Січня ",    " Yanvar "},
  {" February ", " Февраля ", " Februarie ", " Februar ",  " Vasaris ",  " Veebruar ", " Лютого ",   " Fevral "},
  {" March ",    " Марта ",   " Martie ",    " März ",     " Kovas ",    " Märts ",    " Березня ",  " Mart "},
  {" April ",    " Апреля ",  " Aprilie ",   " April ",    " Balandis ", " Aprill ",   " Квітня ",   " Aprel "},
  {" May ",      " Мая ",     " Mai ",       " Mai" ,      " Gegužės ",  " Mai ",      " Травня ",   " May "},
  {" June ",     " Июня ",    " Iunie ",     " Juni ",     " Birželis ", " Juuni ",    " Червня ",   " İyun "},
  {" July ",     " Июля ",    " Iulie ",     " Juli ",     " Liepa ",    " Juuli ",    " Липня ",    " İyul "},
  {" August ",   " Августа ", " August ",    " August ",   " Rugpjūtis "," August ",   " Серпня ",   " Avqust "},
  {" September "," Сентября "," Septembrie "," September "," Rugsėjis ", " September "," Вересня ",  " Sentyabr "},
  {" October ",  " Октября ", " Octombrie ", " Oktober ",  " Spalis ",   " Oktoober ", " Жовтня ",   " Oktyabr "},
  {" November ", " Ноября ",  " Noiembrie ", " November ", " Lapkritis "," November ", " Листопада "," Noyabr "},
  {" December ", " Декабря ", " Decembrie ", " Dezember ", " Gruedis ",  " Detsember "," Грудня ",   " Dekabr "}
};

char* Weekday[7][8]={
  {"Sunday",   "Воскресенье","Duminică","Sonntag",   "Sekmadienis","Pühapäev", "Неділя",   "Bazar"},
  {"Monday",   "Понедельник","Luni",    "Montag",    "Pirmadienis","Esmaspäev","Понеділок","Bazar e-si"},
  {"Tuesday",  "Вторник",    "Marți",   "Dienstag",  "Antradienis","Teisipäev","Вівторок", "Çərşənbə ax"},
  {"Wednesday","Среда",      "Miercuri","Mittwoch",  "Trečiadieni","Kolmapäev","Середа",   "Çərşənbə"},
  {"Thursday", "Четверг",    "Joi",     "Donnerstag","Ketvirtadie","Neljapäev","Четвер",   "Cümə axşamı"},
  {"Friday",   "Пятница",    "Vineri",  "Freitag",   "Penktadieni","Reede",    "Пятниця",  "Cümə"},
  {"Saturday", "Суббота",    "Sâmbătă", "Sonnabend", "Šeštadienis","Laupäev",  "Субота",   "Şənbə"}
};

char* WD[7][8]={
  {"Su","Вс","D", "So","S", "P","Нд","B"},
  {"Mo","Пн","L", "Mo","Pr","E","Пн","Be"},
  {"Tu","Вт","Ma","Di","A", "T","Вт","Ça"},
  {"We","Ср","Mi","Mi","T", "K","Ср","Ç"},
  {"Th","Чт","J", "Do","K", "N","Чт","Ca"},
  {"Fr","Пт","V", "Fr","Pn","R","Пт","C"},
  {"Sa","Сб","S", "Sa","Š", "L","Сб","Ş"}
};

struct Weather_now{
  char* Wind;
  char* meter_sec;
  char* miles_hour;
  char* Humidity;
  char* Pressure;
  char* mm;
  char* hpa;
  char* AM;
  char* PM;
  char* dew;
};

struct Weather_now WeatherNow[8]={
  "Wind ",
  "m/s",
  "mph",
  "Humidity",
  "Pressure",
  "mm",
  "hPa",
  "AM",
  "PM",
  "Dew point ",

  "Ветер ",
  "м/с",
  "м/ч",
  "Влажность",
  "Давление",
  "мм",
  "гПа",
  "ДП",
  "ПП",
  "Точка росы ",

  "Vîntul ",
  "m/s",
  "mph",
  "Umiditatea",
  "Presiunea",
  "mm",
  "hPa",
  "AM",
  "PM",
  "Punct de condensare ",

  "Wind ",
  "m/s",
  "mph",
  "Luftfeuchte",
  "Luftdruck",
  "mm",
  "hPa",
  "AM",
  "PM",
  "Taupunkt ",

  "Vėjas ",
  "m/s",
  "mph",
  "Drėgnumas",
  "Slėgis",
  "mm",
  "hPa",
  "AM",
  "PM",
  "Rasos taškas ",

  "Tuul ",
  "m/s",
  "ml/h",
  "Niiskus",
  "Õhurõhu",
  "mm",
  "hPa",
  "AM",
  "PM",
  "Kastepunkt ",

  "Вітер ",
  "м/с",
  "мл/г",
  "Вологість",
  "Тиск",
  "мм",
  "гПа",
  "ДП",
  "ПП",
  "Точка роси ",

  "Külək ",
  "m/s",
  "ml/s",
  "Nisbi rütubət",
  "Təzyiq",
  "mm",
  "hPa",
  "GƏ",
  "GS",
  "Şeh nöqtəsi ",
};
