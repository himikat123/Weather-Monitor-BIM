import Save from "../Save";
import Languages from "../../Languages";
import { closeMenu } from "../../menu/closeMenu";

class WeatherFn extends Save {
    constructor(props) {
        super(props);
        this.state = {
            provider: ['open-meteo.com', 'weatherbit.io'],
            color: 'text-info',
            temperature: '--',
            humidity: '--',
            pressure: '--',
            wind: '--',
            dir: 0,
            description: '--',
            city: '--',
            country: '',
            latitude: '--',
            longitude: '--',
            checkWeather: '',
            getCoordinates: '',
            menuOpen: 1
        };

        this.changedConfig = this.changedConfig.bind(this);
        this.getGeolocation = this.getGeolocation.bind(this);
        this.getWeather = this.getWeather.bind(this);
        this.openMeteoCode = this.openMeteoCode.bind(this);
    };

    /**
     * Get Geolocation from browser
     */
    getGeolocation() {
        this.setState({getCoordinates: 'spinner-border spinner-border-sm ms-2'});
        if(navigator.geolocation) {
            navigator.geolocation.getCurrentPosition((position) => {
                this.setState({getCoordinates: ''});
                this.props.config.weather.lat = position.coords.latitude;
                this.props.config.weather.lon = position.coords.longitude;
                this.changedConfig(this.props.config);
            });
        }
    }

    openMeteoCode(code) {
        let text = new Languages();
        let lang = this.props.config.lang;
        switch(code) {
            case 0: return text.get('clearSky', lang);
            case 1: return text.get('mainlyClear', lang);
            case 2: return text.get('partlyCloudy', lang);
            case 3:	return text.get('overcast', lang);
            case 45: return text.get('fog', lang);
            case 48: return text.get('deposRimeFog', lang);
            case 51: case 53: case 55: return text.get('drizzle', lang);
            case 56: case 57: return text.get('freezingDrizzle', lang);
            case 61: case 63: case 65: return text.get('rain', lang);
            case 66: case 67: return text.get('freezingRain', lang);
            case 71: case 73: case 75: return text.get('snowFall', lang);
            case 77: return text.get('snowGrains', lang);
            case 80: case 81: case 82: return text.get('rainShowers', lang);
            case 85: case 86: return text.get('snowShowers', lang);
            case 95: return text.get('thunderstorm', lang);
            case 96: case 99: return text.get('thunderstormWithHail', lang);
            default: return "---";
        }
    }

    /**
     * Get weather forecast from internet
     */
    getWeather() {
        this.setState({checkWeather: 'spinner-border spinner-border-sm ms-2'});
        let current = '', citysearch = '';
        let text = new Languages();
        let lang = this.props.config.lang;
        
        // from openmeteo.com
        if(this.props.config.weather.provider == 0) {
            current = 'https://api.open-meteo.com/v1/forecast';
            current += `?latitude=${this.props.config.weather.lat}`;
            current += `&longitude=${this.props.config.weather.lon}`;
            current += '&current=temperature_2m,relative_humidity_2m,pressure_msl,wind_speed_10m,wind_direction_10m,weather_code';
            current += '&wind_speed_unit=ms&timeformat=unixtime&timezone=auto';
        }

        // from weatherbit.io
        if(this.props.config.weather.provider == 1) {
            current = `https://api.weatherbit.io/v2.0/current?key=${this.props.config.weather.appid[1]}`;
            citysearch = `&lat=${this.props.config.weather.lat}&lon=${this.props.config.weather.lon}`;
            current += `${citysearch}`;
            current += `&lang=${this.props.config.lang}`;
        }
        console.log(current);
        fetch(current).then((response) => {
            return response.json();
        }).then((json) => {
            this.setState({checkWeather: ''});
            try {
                // get data from open-meteo.com
                if(this.props.config.weather.provider == 0) {
                    this.setState({
                        color: 'text-info',
                        temperature: json["current"]["temperature_2m"],
                        humidity: json["current"]["relative_humidity_2m"],
                        pressure: Math.round(json["current"]["pressure_msl"] * 0.75),
                        wind: json["current"]["wind_speed_10m"],
                        dir: json["current"]["current.wind_direction_10m"],
                        description: this.openMeteoCode(json["current"]["weather_code"]),
                        city: json["timezone"],
                        latitude: json["latitude"],
                        longitude: json["longitude"]
                    });
                }
                // get data from weatherbit.io
                if(this.props.config.weather.provider == 1) {
                    this.setState({
                        color: 'text-info',
                        temperature: json["data"][0]["temp"],
                        humidity: Math.round(json["data"][0]["rh"]),
                        pressure: Math.round(json["data"][0]["pres"] * 0.75),
                        wind: (json["data"][0]["wind_spd"]).toFixed(1),
                        dir: (json["data"][0]["wind_dir"]),
                        description: json["data"][0]["weather"]["description"],
                        city: json["data"][0]["city_name"],
                        country: json["data"][0]["country_code"],
                        latitude: json["data"][0]["lat"],
                        longitude: json["data"][0]["lon"]
                    });
                }
            }
            catch(e) {
                console.error(e);
                this.setState({
                    color: 'text-danger',
                    temperature: '--',
                    humidity: '--',
                    pressure: '--',
                    wind: '--',
                    dir: 0,
                    description: text.get('weatherCheckError', lang),
                    city: '--',
                    country: '',
                    latitude: '--',
                    longitude: '--'
                });
            }
        });
    }

    componentDidUpdate() {
        if(this.state.menuOpen) {
            closeMenu();
            this.setState({ menuOpen: 0 });
        }
    }
};

export default WeatherFn;