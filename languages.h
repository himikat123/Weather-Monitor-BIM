struct status_text{
  char* connecting_to;
  char* unable_to_connect_to;
  char* connected_to;
  char* network;
  char* not_found;
};

struct status_text status_lng[6]={
  "Connecting to",
  "Unable to connect to",
  "Connected to",
  "Network ",
  " not found",
  
  "Соединение с",
  "Не удается подключиться к",
  "Подключено к",
  "Сеть ",
  " не найдена",
  
  "Conectare cu",
  "Nu se poate conecta la",
  "Conectat la",
  "Retea ",
  " not found",
  
  "Connecting to",
  "Unable to connect to",
  "Connected to",
  "Network ",
  " not found"
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

struct settings_text settings_lng[4]={
  "To enter setup",
  "connect your laptop",
  "tablet or smartphone to",
  " ",
  "password",
  "Open your browser and enter",
  "address 192.168.4.1",

  "Чтоб войти в настройки",
  "подключите ваш ноутбук,",
  "планшет или смартфон к",
  "сети",
  "пароль",
  "Откройте браузер и введите",
  "адрес 192.168.4.1",

  "Pentru a intra în setări",
  "conectați laptopul,",
  "tableta sau smartphone",
  "la rețeaua",
  "parola",
  "și introduceți adresa",
  "192.168.4.1 în browser",

  "So geben sie setup",
  "schließen sie Ihren laptop",
  "tablet oder smartphone",
  "network",
  "passwort",
  "Öffnen sie Ihren browser",
  "und geben Sie adresse 192.168.4.1",
};

struct web_text{
  const char* Settings;
  const char* YourSSID;  
  const char* YourPASS;
  const char* YourCity;  
  const char* YourTimeZone;  
  const char* AdjustClock; 
  const char* Yes;
  const char* No;
  const char* Lang;
  const char* Language;
  const char* Measures;
  const char* Units;
  const char* Metric;
  const char* title_metric;
  const char* Imperial;
  const char* title_imperial;
  const char* TimeFormat;
  const char* Twelve;
  const char* TwentyFour;
  const char* Brightness;
  const char* SaveAll;  
  const char* Saved;
  const char* show_password;
  const char* ap_ssid;
  const char* ap_pass;
  const char* sleep;
  const char* never;
  const char* after;
  const char* min;
  const char* type;
  const char* dyn;
  const char* stat;
  const char* ip;
  const char* mask;
  const char* gateway;
  const char* ip_err;
  const char* mask_err;
  const char* gw_err;
};

struct web_text web_lng[4]={
  "Settings",
  "Your WiFi network",
  "Your password",
  "Your city name",
  "Your time zone",
  "Automaticaly adjust clock for Daylight Saving Time",
  "Yes",
  "No",
  "Language",
  "English",
  "Measures",
  "Units",
  "Metric",
  "temperature Celsius, wind m/s",
  "Imperial",
  "temperature Fahrenheit, wind mph",
  "Time format",
  "12 hour",
  "24 hour",
  "Brightness",
  "Save all",
  "Saved",
  "show password",
  "Access point name",
  "Access point password",
  "Sleep",
  "never",
  "after",
  "min",
  "Connection type",
  "Dynamic IP",
  "Static IP",
  "IP address",
  "Subnet mask",
  "Default gateway",
  "IP address is incorrect",
  "subnet mask is incorrect",
  "default gateway is incorrect",
  
  "Настройки",
  "Ваша WiFi сеть",
  "Пароль вашей сети",
  "Ваш город",
  "Часовой пояс",  
  "Автоматический переход на летнее время", 
  "Да",
  "Нет",
  "Язык",
  "Русский",
  "Единицы измерения",
  "Система",
  "Метрическая",
  "температура в Цельсиях, ветер в метр/сек",
  "Имперская",
  "температура в Фаренгейтах, ветер в миль/час",
  "Формат часов",
  "12 часовой",
  "24 часовой",
  "Яркость",
  "Сохранить все",
  "Сохранено",
  "показать пароль",
  "Имя точки доступа",
  "Пароль точки доступа",
  "Режим сна",
  "никогда",
  "через",
  "мин",
  "Тип соединения",
  "Динамический IP адрес",
  "Статический IP адрес",
  "IP адрес",
  "Маска подсети",
  "Основной шлюз",
  "неверный IP адрес",
  "неверная маска подсети",
  "неверный адрес шлюза",

  "Setări",
  "Rețeaua Dvs WiFi",
  "Parola rețelei Dvs",
  "Orașul Dvs",
  "Fusul orar",  
  "Ajustarea automată a ceasului la modificarea orei de vară", 
  "Da",
  "Nu",
  "Limba",
  "Română",
  "Unitatea de măsură",
  "Sistemul",
  "Metric",
  "temperatura Celsius, vântul m/s",
  "Imperial",
  "temperatura Farengeit, vântul m/s",
  "Formatul timpului",
  "12 ore",
  "24 ore",
  "Luminozitatea",
  "Salvează tot",
  "Salvat",
  "show password",
  "Access point name",
  "Access point password",
  "Sleep",
  "never",
  "after",
  "min",
  "Connection type",
  "Dynamic IP",
  "Static IP",
  "IP address",
  "Subnet mask",
  "Default gateway",
  "IP address is incorrect",
  "subnet mask is incorrect",
  "default gateway is incorrect",

  "Einstellungen",
  "Ihr WiFi-Netzwerk",
  "Ihr Passwort",
  "Dein Stadtname",
  "Ihre Zeitzone",
  "Anpassung der Uhr automatisch für Sommerzeit",
  "Ja",
  "Nein",
  "Sprache",
  "Deutsch",
  "Measures",
  "Units",
  "Metric",
  "temperature Celius, wind m/s",
  "Imperial",
  "temperature Fahrenheit, wind m/s",
  "Time format",
  "12 hour",
  "24 hour",
  "Brightness",
  "Save all",
  "Saved",
  "show password",
  "Access point name",
  "Access point password",
  "Sleep",
  "never",
  "after",
  "min",
  "Connection type",
  "Dynamic IP",
  "Static IP",
  "IP address",
  "Subnet mask",
  "Default gateway",
  "IP address is incorrect",
  "subnet mask is incorrect",
  "default gateway is incorrect"
};

struct Month{
  char* January;
  char* February;
  char* March;
  char* April;
  char* May;
  char* June;
  char* July;
  char* August;
  char* September;
  char* October;
  char* November;
  char* December;
};

struct Month Mon_lng[4]={
  "January",
  "February",
  "March",
  "April",
  "May",
  "June",
  "July",
  "August",
  "September",
  "October",
  "November",
  "December",
  
  "Января",
  "Февраля",
  "Марта",
  "Апреля",
  "Мая",
  "Июня",
  "Июля",
  "Августа",
  "Сентября",
  "Октября",
  "Ноября",
  "Декабря",

  "Ianuarie",
  "Februarie",
  "Martie",
  "Aprilie"
  "Mai",
  "Iunie",
  "Iulie",
  "August",
  "Septembrie",
  "Octombrie",
  "Noiembrie",
  "Decembrie",
  
  "Januar",
  "Februar",
  "Marz",
  "April",
  "Mai",
  "Juni",
  "Juli",
  "August",
  "September",
  "Oktober",
  "November",
  "Dezember"
};

struct Weekday{
  char* Sunday;
  char* Monday;
  char* Tuesday;
  char* Wednesday;
  char* Thursday;
  char* Friday;
  char* Saturday;
};

struct Weekday Week_lng[4]={
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday",

  "Воскресенье",
  "Понедельник",
  "Вторник",
  "Среда",
  "Четверг",
  "Пятница",
  "Суббота",

  "Duminică",
  "Luni",
  "Marți",
  "Miercuri",
  "Joi",
  "Vineri",
  "Sâmbătă",

  "Sonntag",
  "Montag",
  "Dienstag",
  "Mittwoch",
  "Donnerstag",
  "Freitag",
  "Sonnabend"
};

struct Weather_now{
  char* Now;
  char* Wind;
  char* meter_sec;
  char* miles_hour;
  char* Humidity;
  char* Pressure;
  char* mm;
  char* hpa;
  char* AM;
  char* PM;
};

struct Weather_now WeatherNow[4]={
  "Now",
  "Wind ",
  "m/s",
  "mph",
  "Humidity ",
  "Pressure ",
  "mmHg",
  "hPa",
  "AM",
  "PM",

  "Сейчас",
  "Ветер ",
  "м/с",
  "м/ч",
  "Влажность ",
  "Давление ",
  "мм рт.ст.",
  "гПа",
  "ДП",
  "ПП",

  "La moment",
  "Vîntul ",
  "m/s",
  "mph",
  "Umiditatea ",
  "Presiunea ",
  "mmHg",
  "hPa",
  "AM",
  "PM",

  "Jetzt",
  "Wind ",
  "m/s",
  "mph",
  "Luftfeuchte ",
  "Luftdruck ",
  "mmHg",
  "hPa",
  "AM",
  "PM"
};

struct Weather_daily{
  char* Today;
  char* Tomorrow;
  char* AfterTomorrow;
};

struct Weather_daily WeatherDaily[4]={
  "Today",
  "Tomorrow",
  "AfterTomorrow",

  "Сегодня",
  "Завтра",
  "Послезавтра",

  "Astăzi",
  "Mâine",
  "Poimâine",
  
  "Heute",
  "Morgen",
  "Übermorgen",
};
