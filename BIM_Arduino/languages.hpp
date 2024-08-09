class Lang {
  private:
    String _day() {
      if(config.lang() == "ru") return "день";
      return "day";
    }
    
    String _days2() {
      if(config.lang() == "ru") return "дня";
      return "days";  
    }
    
    String _days5() {
      if(config.lang() == "ru") return "дней";
      return "days";  
    }

    String _hour() {
      if(config.lang() == "ru") return "час";
      return "hour";  
    }
    
    String _hours2() {
      if(config.lang() == "ru") return "часа";
      return "hours";  
    }
    
    String _hours5() {
      if(config.lang() == "ru") return "часов";
      return "hours";  
    }

    String _minute() {
      if(config.lang() == "ru") return "минута";
      return "minute";
    }

    String _minutes2() {
      if(config.lang() == "ru") return "минуты";
      return "minutes";  
    }

    String _minutes5() {
      if(config.lang() == "ru") return "минут";
      return "minutes";  
    }

    String _lessThanAMinute() {
      if(config.lang() == "ru") return "Меньше минуты";
      return "Less than a minute";  
    }

    String _and() {
      if(config.lang() == "ru") return "и";
      return "and";  
    }

  public:
    String mo() {
      if(config.lang() == "ru") return "Пн";
      return "Mo";  
    }

    String tu() {
      if(config.lang() == "ru") return "Вт";
      else return "Tu";  
    }

    String we() {
      if(config.lang() == "ru") return "Ср";
      return "We";  
    }

    String th() {
      if(config.lang() == "ru") return "Чт";
      return "Th";  
    }

    String fr() {
      if(config.lang() == "ru") return "Пт";
      return "Fr";  
    }

    String sa() {
      if(config.lang() == "ru") return "Сб";
      return "Sa";  
    }

    String su() {
      if(config.lang() == "ru") return "Вс";
      return "Su";  
    }
    
    String mm() { // millimeters
      if(config.lang() == "ru") return "мм";
      return "mm";  
    }

    String ms() { // meters per second
      if(config.lang() == "ru") return "м/с";
      return "m/s";  
    }

    String v() { // volt
      if(config.lang() == "ru") return "В";
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
          if(config.lang() == "ru") return "Комфортно";
          return "Comfortable";
        }; break;
        case 2: {
          if(config.lang() == "ru") return "Слишком жарко";
          return "Too hot";
        }; break;
        case 3: {
          if(config.lang() == "ru") return "Слишком холодно";
          return "Too cold";
        }; break;
        case 4: {
          if(config.lang() == "ru") return "Слишком сухо";
          return "Too dry";
        }; break;
        case 5: {
          if(config.lang() == "ru") return "Слишком влажно";
          return "Too humid";
        }; break;
        case 6: {
          if(config.lang() == "ru") return "Жарко и влажно";
          return "Hot and humid";
        }; break;
        case 7: {
          if(config.lang() == "ru") return "Жарко и сухо";
          return "Hot and dry";
        }; break;
        case 8: {
          if(config.lang() == "ru") return "Холодно и влажно";
          return "Cold and humid";
        }; break;
        case 9: {
          if(config.lang() == "ru") return "Холодно и сухо";
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
      if(config.lang() == "ru") return "Ясно";
      return "Clear sky";
    }
        
    String mainlyClear() {
      if(config.lang() == "ru") return "Преимущественно ясно";
      return "Mainly clear";
    }

    String partlyCloudy() {
      if(config.lang() == "ru") return "Переменная облачность";
      return "Partly cloudy";
    }

    String overcast() {
      if(config.lang() == "ru") return "Пасмурно";
      return "Overcast";
    }

    String fog() {
      if(config.lang() == "ru") return "Туман";
      return "Fog";
    }

    String deposRimeFog() {
      if(config.lang() == "ru") return "Туман с инеем";
      return "Depositing rime fog";
    }

    String drizzle() {
      if(config.lang() == "ru") return "Морось";
      return "Drizzle";
    }

    String freezingDrizzle() {
      if(config.lang() == "ru") return "Изморозь";
      return "Freezing drizzle";
    }

    String rain() {
      if(config.lang() == "ru") return "Дождь";
      return "Rain";
    }

    String freezingRain() {
      if(config.lang() == "ru") return "Ледяной дождь";
      return "Freezing rain";
    }

    String snowFall() {
      if(config.lang() == "ru") return "Снегопад";
      return "Snowfall";
    }

    String snowGrains() {
      if(config.lang() == "ru") return "Снежные зерна";
      return "Snow grains";
    }

    String rainShowers() {
      if(config.lang() == "ru") return "Ливень";
      return "Rain showers";
    }

    String snowShowers() {
      if(config.lang() == "ru") return "Сильный снегопад";
      return "Snow showers";
    }

    String thunderstorm() {
      if(config.lang() == "ru") return "Гроза";
      return "Thunderstorm";
    }

    String thunderstormWithHail() {
      if(config.lang() == "ru") return "Гроза с градом";
      return "Thunderstorm with hail";
    }
};
