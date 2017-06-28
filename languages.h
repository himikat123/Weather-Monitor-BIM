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
  "Jan",
  "Feb",
  "March",
  "April",
  "May",
  "June",
  "July",
  "August",
  "Sept",
  "Oct",
  "Nov",
  "Decr",
    
  "Января",
  "Февр",
  "Марта",
  "Апреля",
  "Мая",
  "Июня",
  "Июля",
  "Авг",
  "Сент",
  "Окт",
  "Ноября",
  "Дек",

  "Ian",
  "Feb",
  "Martie",
  "Apr",
  "Mai",
  "Iunie",
  "Iulie",
  "August",
  "Sept",
  "Oct",
  "Noiemb",
  "Dec",

  "Januar",
  "Feb",
  "März",
  "April",
  "Mai",
  "Juni",
  "Juli",
  "August",
  "Sep",
  "Okt",
  "Nov",
  "Dez"
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
  "Wind",
  "m/s",
  "mph",
  "Humidity",
  "Pressure",
  "mmHg",
  "hPa",
  "AM",
  "PM",

  "Сейчас",
  "Ветер",
  "м/с",
  "м/ч",
  "Влажность",
  "Давление",
  "мм рт.ст",
  "гПа",
  "ДП",
  "ПП",

  "La moment",
  "Vîntul",
  "m/s",
  "mph",
  "Umiditatea",
  "Presiunea",
  "mmHg",
  "hPa",
  "AM",
  "PM",

  "Jetzt",
  "Wind",
  "m/s",
  "mph",
  "Luftfeuchte",
  "Luftdruck",
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
