class Lang {
  private:
    String _day() {
      if(config.lang() == "ru") return "день";
      if(config.lang() == "bg") return "ден";
      return "day";
    }
    
    String _days2() {
      if(config.lang() == "ru") return "дня";
      if(config.lang() == "bg") return "дни";
      return "days";  
    }
    
    String _days5() {
      if(config.lang() == "ru") return "дней";
      if(config.lang() == "bg") return "дни";
      return "days";  
    }

    String _hour() {
      if(config.lang() == "ru" or config.lang() == "bg") return "час";
      return "hour";  
    }
    
    String _hours2() {
      if(config.lang() == "ru" or config.lang() == "bg") return "часа";
      return "hours";  
    }
    
    String _hours5() {
      if(config.lang() == "ru") return "часов";
      if(config.lang() == "bg") return "часа";
      return "hours";  
    }

    String _minute() {
      if(config.lang() == "ru" or config.lang() == "bg") return "минута";
      return "minute";
    }

    String _minutes2() {
      if(config.lang() == "ru") return "минуты";
      if(config.lang() == "bg") return "минути";
      return "minutes";  
    }

    String _minutes5() {
      if(config.lang() == "ru") return "минут";
      if(config.lang() == "bg") return "минути";
      return "minutes";  
    }

    String _lessThanAMinute() {
      if(config.lang() == "ru") return "Меньше минуты";
      if(config.lang() == "bg") return "По-малко от минута";
      return "Less than a minute";  
    }

    String _and() {
      if(config.lang() == "ru" or config.lang() == "bg") return "и";
      return "and";  
    }

  public:
    String mo() {
      if(config.lang() == "ru") return "Пн";
      if(config.lang() == "bg") return "По";
      return "Mo";  
    }

    String tu() {
      if(config.lang() == "ru" or config.lang() == "bg") return "Вт";
      else return "Tu";  
    }

    String we() {
      if(config.lang() == "ru" or config.lang() == "bg") return "Ср";
      return "We";  
    }

    String th() {
      if(config.lang() == "ru") return "Чт";
      if(config.lang() == "bg") return "Че";
      return "Th";  
    }

    String fr() {
      if(config.lang() == "ru") return "Пт";
      if(config.lang() == "bg") return "Пе";
      return "Fr";  
    }

    String sa() {
      if(config.lang() == "ru") return "Сб";
      if(config.lang() == "bg") return "Съ";
      return "Sa";  
    }

    String su() {
      if(config.lang() == "ru") return "Вс";
      if(config.lang() == "bg") return "Не";
      return "Su";  
    }
    
    String mm() { // millimeters
      if(config.lang() == "ru" or config.lang() == "bg") return "мм";
      return "mm";  
    }

    String ms() { // meters per second
      if(config.lang() == "ru" or config.lang() == "bg") return "м/с";
      return "m/s";  
    }

    String v() { // volt
      if(config.lang() == "ru" or config.lang() == "bg") return "В";
      return "V";
    }

    unsigned int plural(int num) {
      if(num > 10 && num < 20) return 0;
      if(num % 10 == 1) return 1;
      if(num % 10 > 1 && num % 10 < 5) return 2;
      return 0;
    }
    
    String runtime(unsigned long mil) {
      String buf = "";

      if(mil < 60) buf = _lessThanAMinute();
      else if(mil < 3600) {
        buf = String(minute(mil)) + " ";
        buf += (plural(minute(mil)) == 1) ? _minute() : (plural(minute(mil)) == 2) ? _minutes2() : _minutes5();
      }
      else if(mil < 86400) {
        buf = String(hour(mil)) + " "; 
        buf += ((plural(hour(mil)) == 1) ? _hour() : (plural(hour(mil)) == 2) ? _hours2() : _hours5()) + " ";
        buf += _and() + " ";  
        buf += String(minute(mil)) + " ";
        buf += (plural(minute(mil)) == 1) ? _minute() : (plural(minute(mil)) == 2) ? _minutes2() : _minutes5();
      }
      else {
        int days = lround(mil / 86400);
        buf = String(days) + " ";
        buf += ((plural(days) == 1) ? _day() : (plural(days) == 2) ? _days2() : _days5()) + ", ";
        buf += String(hour(mil)) + " ";
        buf += ((plural(hour(mil)) == 1) ? _hour() : (plural(hour(mil)) == 2) ? _hours2() : _hours5()) + " ";
        buf += _and() + " ";
        buf += String(minute(mil)) + " ";
        buf += (plural(minute(mil)) == 1) ? _minute() : (plural(minute(mil)) == 2) ? _minutes2() : _minutes5();
      }
      
      return buf;
    }

    String comfort(unsigned int level) {
      switch(level) {
        case 1: {
          if(config.lang() == "ru" or config.lang() == "bg") return "Комфортно";
          return "Comfortable";
        }; break;
        case 2: {
          if(config.lang() == "ru") return "Слишком жарко";
          if(config.lang() == "bg") return "Горещо";
          return "Too hot";
        }; break;
        case 3: {
          if(config.lang() == "ru") return "Слишком холодно";
          if(config.lang() == "bg") return "Студено";
          return "Too cold";
        }; break;
        case 4: {
          if(config.lang() == "ru") return "Слишком сухо";
          if(config.lang() == "bg") return "Твърде сухо";
          return "Too dry";
        }; break;
        case 5: {
          if(config.lang() == "ru") return "Слишком влажно";
          if(config.lang() == "bg") return "Твърде влажно";
          return "Too humid";
        }; break;
        case 6: {
          if(config.lang() == "ru") return "Жарко и влажно";
          if(config.lang() == "bg") return "Горещо и влажно";
          return "Hot and humid";
        }; break;
        case 7: {
          if(config.lang() == "ru") return "Жарко и сухо";
          if(config.lang() == "bg") return "Горещо и сухо";
          return "Hot and dry";
        }; break;
        case 8: {
          if(config.lang() == "ru") return "Холодно и влажно";
          if(config.lang() == "bg") return "Студено и влажно";
          return "Cold and humid";
        }; break;
        case 9: {
          if(config.lang() == "ru") return "Холодно и сухо";
          if(config.lang() == "bg") return "Студено и сухо";
          return "Cold and dry";
        }; break;
        default: return "---"; break;
      }
    }

    String weekdayShortName(unsigned int day) {
      switch(day) {
        case 1: return su();
        case 2: return mo();
        case 3: return tu();
        case 4: return we();
        case 5: return th();
        case 6: return fr();
        case 7: return sa();
        default: return su(); break;
      }
    }

    String clearSky() {
      if(config.lang() == "ru" or config.lang() == "bg") return "Ясно";
      return "Clear sky";
    }
        
    String mainlyClear() {
      if(config.lang() == "ru") return "Преимущественно ясно";
      if(config.lang() == "bg") return "Предимно ясно";
      return "Mainly clear";
    }

    String partlyCloudy() {
      if(config.lang() == "ru") return "Переменная облачность";
      if(config.lang() == "bg") return "Предимно облачно";
      return "Partly cloudy";
    }

    String overcast() {
      if(config.lang() == "ru") return "Пасмурно";
      if(config.lang() == "bg") return "Облачно";
      return "Overcast";
    }

    String fog() {
      if(config.lang() == "ru") return "Туман";
      if(config.lang() == "bg") return "Мъгла";
      return "Fog";
    }

    String deposRimeFog() {
      if(config.lang() == "ru") return "Туман с инеем";
      if(config.lang() == "bg") return "Мъгла със скреж";
      return "Depositing rime fog";
    }

    String drizzle() {
      if(config.lang() == "ru") return "Морось";
      if(config.lang() == "bg") return "Дъжд";
      return "Drizzle";
    }

    String freezingDrizzle() {
      if(config.lang() == "ru") return "Изморозь";
      if(config.lang() == "bg") return "Скреж";
      return "Freezing drizzle";
    }

    String rain() {
      if(config.lang() == "ru") return "Дождь";
      if(config.lang() == "bg") return "Дъжд";
      return "Rain";
    }

    String freezingRain() {
      if(config.lang() == "ru") return "Ледяной дождь";
      if(config.lang() == "bg") return "Леден дъжд";
      return "Freezing rain";
    }

    String snowFall() {
      if(config.lang() == "ru") return "Снегопад";
      if(config.lang() == "bg") return "Снеговалеж";
      return "Snowfall";
    }

    String snowGrains() {
      if(config.lang() == "ru") return "Снежные зерна";
      if(config.lang() == "bg") return "Скреж";
      return "Snow grains";
    }

    String rainShowers() {
      if(config.lang() == "ru") return "Ливень";
      if(config.lang() == "bg") return "Ръмеж";
      return "Rain showers";
    }

    String snowShowers() {
      if(config.lang() == "ru") return "Сильный снегопад";
      if(config.lang() == "bg") return "Обилен снеговалеж";
      return "Snow showers";
    }

    String thunderstorm() {
      if(config.lang() == "ru") return "Гроза";
      if(config.lang() == "bg") return "Буря";
      return "Thunderstorm";
    }

    String thunderstormWithHail() {
      if(config.lang() == "ru") return "Гроза с градом";
      if(config.lang() == "bg") return "Буря с градушка";
      return "Thunderstorm with hail";
    }
};