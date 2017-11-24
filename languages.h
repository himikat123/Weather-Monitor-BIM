struct saved_text{
  char* saved;
  char* not_saved;
};

struct saved_text saved[7]={
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
  "Не збережено"
};

struct status_text{
  char* connecting_to;
  char* unable_to_connect_to;
  char* connected_to;
  char* network;
  char* not_found;
};

struct status_text status_lng[7]={
  "Connecting to",
  "Unable to connect to the network",
  "Connected to",
  "Network ",
  " not found",
  
  "Соединение с",
  "Не удается подключиться к сети",
  "Подключено к",
  "Сеть ",
  " не найдена",
  
  "Conectare cu",
  "Nu se poate conecta la rețea",
  "Conectat la",
  "Rețeaua ",
  " nu a fost găsită",
  
  "Verbinden mit",
  "keine Verbindung zum Netzwerk",
  "Angeschlossen",
  "Netzwerk ",
  " nicht gefunden",

  "Prisijungiama prie",
  "Nepavyko prisijungti prie tinklo",
  "Sujungta su",
  "Tinklas ",
  " nerastas",

  "Ühend",
  "Võrku ei saa ühendust luua",
  "Ühendatud",
  "Võrk ",
  " ei leitud",

  "З’єднання з",
  "Не вдається підключитись до мережі",
  "Підключено до",
  "Мережу ",
  " не знайдено"
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

struct settings_text settings_lng[7]={
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
  "адресу http://192.168.4.1"
};

char* Month[12][7]={
  {" January ",  " Января ",  " Ianuarie ",  " Januar ",   " Sausis ",   " Jaanuar ",  " Січня "},
  {" February ", " Февраля ", " Februarie ", " Februar ",  " Vasaris ",  " Veebruar ", " Лютого "},
  {" March ",    " Марта ",   " Martie ",    " März ",     " Kovas ",    " Märts ",    " Березня "},
  {" April ",    " Апреля ",  " Aprilie ",   " April ",    " Balandis ", " Aprill ",   " Квітня "},
  {" May ",      " Мая ",     " Mai ",       " Mai" ,      " Gegužės ",  " Mai ",      " Травня "},
  {" June ",     " Июня ",    " Iunie ",     " Juni ",     " Birželis ", " Juuni ",    " Червня "},
  {" July ",     " Июля ",    " Iulie ",     " Juli ",     " Liepa ",    " Juuli ",    " Липня "},
  {" August ",   " Августа ", " August ",    " August ",   " Rugpjūtis "," August ",   " Серпня "},
  {" September "," Сентября "," Septembrie "," September "," Rugsėjis ", " September "," Вересня "},
  {" October ",  " Октября ", " Octombrie ", " Oktober ",  " Spalis ",   " Oktoober ", " Жовтня "},
  {" November ", " Ноября ",  " Noiembrie ", " November ", " Lapkritis "," November ", " Листопада "},
  {" December ", " Декабря ", " Decembrie ", " Dezember ", " Gruedis ",  " Detsember "," Грудня "}
};

char* Weekday[7][7]={
  {"Sunday",   "Воскресенье","Duminică","Sonntag",   "Sekmadienis","Pühapäev", "Неділя"},
  {"Monday",   "Понедельник","Luni",    "Montag",    "Pirmadienis","Esmaspäev","Понеділок"},
  {"Tuesday",  "Вторник",    "Marți",   "Dienstag",  "Antradienis","Teisipäev","Вівторок"},
  {"Wednesday","Среда",      "Miercuri","Mittwoch",  "Trečiadieni","Kolmapäev","Середа"},
  {"Thursday", "Четверг",    "Joi",     "Donnerstag","Ketvirtadie","Neljapäev","Четвер"},
  {"Friday",   "Пятница",    "Vineri",  "Freitag",   "Penktadieni","Reede",    "Пятниця"},
  {"Saturday", "Суббота",    "Sâmbătă", "Sonnabend", "Šeštadienis","Laupäev",  "Субота"}
};

char* WD[7][7]={
  {"Su","Вс","D", "So","S", "P","Нд"},
  {"Mo","Пн","L", "Mo","Pr","E","Пн"},
  {"Tu","Вт","Ma","Di","A", "T","Вт"},
  {"We","Ср","Mi","Mi","T", "K","Ср"},
  {"Th","Чт","J", "Do","K", "N","Чт"},
  {"Fr","Пт","V", "Fr","Pn","R","Пт"},
  {"Sa","Сб","S", "Sa","Š", "L","Сб"}
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

struct Weather_now WeatherNow[7]={
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
  "Точка роси "
};
