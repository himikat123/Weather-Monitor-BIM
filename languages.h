struct saved_text{
  char* saved; char* not_saved; const char* old_pass;
};

struct saved_text saved[10]={
  "Saved","Did not save","Invalid old password",
  "Сохранено","Не сохранено","Неверный старый пароль",
  "Salvat","Nu este salvat","Parola veche nevalidă",
  "Gerettet","Nicht gespeichert","Ungültiges altes Passwort",
  "Išsaugota","Nepasaugotas","Netinkamas senas slaptažodis",
  "Salvestatud","Ei salvestatud","Kehtetu vana parool",
  "Збережено","Не збережено","Невірний старий пароль",
  "Yadda saxlanıldı","Yadda saxlanılmadı","Yanlış köhnə parol",
  "Захавана","Не захавана","Няправільны стары пароль",
  "Запазено","Незапазено","Невалидна стара парола"
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

struct status_text status_lng[10]={
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
  "Sensora qoşulur",

  "Злучэнне з",
  "Немагчыма падлучыцца да сетки",
  "Падлучанае да",
  "Сетка ",
  " не знойдзена",
  "Не ўдаецца падключыцца да датчыкаў",
  "Падключэнне да датчыкаў",

  "Връзка с",
  "Не можа да се свърже с мрежата",
  "Свързано с",
  "Мрежа ",
  " не е намерена",
  "Не можа да се свърже със сензора",
  "Свързване към сензора"
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

struct settings_text settings_lng[10]={
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
  "ünvanını daxil edin",

  "Каб увайсці ў налады",
  "падключыце ваш ноўтбук,",
  "планшэт ці смартфон",
  "да сеткі",
  "пароль",
  "Адкрыйце браўзэр і ўвядзіце",
  "адрас http://192.168.4.1",
  
  "За да въведете настройките",
  "свържете лаптопа,",
  "таблета или смарт телефона",
  "си с мрежата",
  "парола",
  "Отворете браузър и",
  "въведете http://192.168.4.1"
};

char* Month[12][10]={
  {" January ",  " Января ",  " Ianuarie ",  " Januar ",   " Sausis ",   " Jaanuar ",  " Січня ",    " Yanvar ",   " Студзеня ",   " Януари "},
  {" February ", " Февраля ", " Februarie ", " Februar ",  " Vasaris ",  " Veebruar ", " Лютого ",   " Fevral ",   " Лютага ",     " Февруари "},
  {" March ",    " Марта ",   " Martie ",    " März ",     " Kovas ",    " Märts ",    " Березня ",  " Mart ",     " Сакавіка ",   " Март "},
  {" April ",    " Апреля ",  " Aprilie ",   " April ",    " Balandis ", " Aprill ",   " Квітня ",   " Aprel ",    " Красавіка ",  " Април "},
  {" May ",      " Мая ",     " Mai ",       " Mai" ,      " Gegužės ",  " Mai ",      " Травня ",   " May ",      " Мая ",        " Май "},
  {" June ",     " Июня ",    " Iunie ",     " Juni ",     " Birželis ", " Juuni ",    " Червня ",   " İyun ",     " Чэрвеня ",    " Юни "},
  {" July ",     " Июля ",    " Iulie ",     " Juli ",     " Liepa ",    " Juuli ",    " Липня ",    " İyul ",     " Ліпеня ",     " Юли "},
  {" August ",   " Августа ", " August ",    " August ",   " Rugpjūtis "," August ",   " Серпня ",   " Avqust ",   " Жніўня ",     " Август "},
  {" September "," Сентября "," Septembrie "," September "," Rugsėjis ", " September "," Вересня ",  " Sentyabr ", " Верасня ",    " Септември "},
  {" October ",  " Октября ", " Octombrie ", " Oktober ",  " Spalis ",   " Oktoober ", " Жовтня ",   " Oktyabr ",  " Кастрычніка "," Октомври "},
  {" November ", " Ноября ",  " Noiembrie ", " November ", " Lapkritis "," November ", " Листопада "," Noyabr ",   " Лістапада ",  " Ноември "},
  {" December ", " Декабря ", " Decembrie ", " Dezember ", " Gruedis ",  " Detsember "," Грудня ",   " Dekabr ",   " Снежня ",     " Декември "}
};

char* Weekday[7][10]={
  {"Sunday",   "Воскресенье","Duminică","Sonntag",   "Sekmadienis","Pühapäev", "Неділя",   "Bazar",      "Нядзеля",   "Неделя"},
  {"Monday",   "Понедельник","Luni",    "Montag",    "Pirmadienis","Esmaspäev","Понеділок","Bazar e-si", "Панядзелак","Понеделник"},
  {"Tuesday",  "Вторник",    "Marți",   "Dienstag",  "Antradienis","Teisipäev","Вівторок", "Çərşənbə ax","Аўторак",   "Вторник"},
  {"Wednesday","Среда",      "Miercuri","Mittwoch",  "Trečiadieni","Kolmapäev","Середа",   "Çərşənbə",   "Серада",    "Сряда"},
  {"Thursday", "Четверг",    "Joi",     "Donnerstag","Ketvirtadie","Neljapäev","Четвер",   "Cümə axşamı","Чацвер",    "Четвъртък"},
  {"Friday",   "Пятница",    "Vineri",  "Freitag",   "Penktadieni","Reede",    "Пятниця",  "Cümə",       "Пятніца",   "Петък"},
  {"Saturday", "Суббота",    "Sâmbătă", "Sonnabend", "Šeštadienis","Laupäev",  "Субота",   "Şənbə",      "Субота",    "Събота"}
};

char* WD[7][10]={
  {"Su","Вс","D", "So","S", "P","Нд","B", "Нд","Нд"},
  {"Mo","Пн","L", "Mo","Pr","E","Пн","Be","Пн","Пн"},
  {"Tu","Вт","Ma","Di","A", "T","Вт","Ça","Аў","Вт"},
  {"We","Ср","Mi","Mi","T", "K","Ср","Ç", "Ср","Ср"},
  {"Th","Чт","J", "Do","K", "N","Чт","Ca","Чц","Чт"},
  {"Fr","Пт","V", "Fr","Pn","R","Пт","C", "Пт","Пт"},
  {"Sa","Сб","S", "Sa","Š", "L","Сб","Ş", "Сб","Сб"}
};

struct Weather_now{
  char* Wind; char* meter_sec; char* km_hour; char* miles_hour;
  char* knots; char* mm; char* hpa; char* AM; char* PM; char* srise; char* sset;
};

struct Weather_now WeatherNow[10]={
  "Wind ","mps","kmph","mph","kn","mm","hPa","AM","PM","sunrise","sunset",
  "Ветер ","м/с","км/ч","м/ч","уз","мм","гПа","ДП","ПП","восход","закат",
  "Vîntul ","m/s","km/h","mph","nod","mm","hPa","AM","PM","răsărit","apus de soare",
  "Wind ","m/s","km/h","mph","kn","mm","hPa","AM","PM","Sonnenaufgang","Sonnenuntergang",
  "Vėjas ","m/s","km/h","mph","kn","mm","hPa","AM","PM","sunrise","sunset",
  "Tuul ","m/s","km/h","ml/h","sõl","mm","hPa","AM","PM","päikesetõusu","päikeseloojangut",
  "Вітер ","м/с","км/г","мл/г","вуз","мм","гПа","ДП","ПП","схід сонця","захід сонця",
  "Külək ","m/s","km/s","ml/s","düy","mm","hPa","GƏ","GS","günəş doğuşu","gün batışı",
  "Вецер ","м/с","км/г","мл/г","вуз","мм","гПа","ДП","ПП","узыход сонца","закат сонца",
  "Вятър","м/с","км/ч","мл/ч","въз","мм","hPa","АМ","РМ","изгрев","залез"
};
