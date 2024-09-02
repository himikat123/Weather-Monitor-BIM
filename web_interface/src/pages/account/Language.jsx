import LanguageFn from "./LanguageFn";
import Languages from "../../Languages";
import PageHeader from "../../fragments/PageHeader";
import {ReactComponent as FlagEn} from "../../svg/en.svg";
import {ReactComponent as FlagRu} from "../../svg/ru.svg";
import {ReactComponent as FlagBg} from "../../svg/bg.svg";
import Menu from "../../menu/Menu";
import SaveRestart from "../SaveRestart";

class Language extends LanguageFn {
    render() {
        let text = new Languages();
        let lang = this.props.config.lang;

        return (<>
            <Menu language={lang} />
            
            <div className="d-flex justify-content-center">
                <div className="content m-2 m-lg-5">
                    <PageHeader h="2" text={text.get('language', lang)} />

                    {this.props.config.hasOwnProperty('account') && 
                    <div className="row d-flex justify-content-center">
                        <div className="col border border-secondary rounded m-4 p-4 account">
                            
                            {/* Language change */}

                            {/* English */}
                            <div className="row my-3">
                                <div className="col-2 pt-1">
                                    <input id="en" className="form-check-input" name="lang" value="en" type="radio" 
                                      checked={this.props.config.lang === "en"} onChange={this.changedLang}
                                    />
                                </div>
                                <div className="col-3">
                                    <label htmlFor="en" className="flag"><FlagEn /></label>
                                </div>
                                <div className="col-7 pt-1">
                                    <label htmlFor="en">English</label>
                                </div>
                            </div>

                            {/* Русский */}
                            <div className="row my-3">
                                <div className="col-2 pt-1">
                                    <input id="ru" className="form-check-input" name="lang" value="ru" type="radio"
                                      checked={this.props.config.lang === "ru"} onChange={this.changedLang}
                                    />
                                </div>
                                <div className="col-3">
                                    <label htmlFor="ru" className="flag"><FlagRu /></label>
                                </div>
                                <div className="col-7 pt-1">
                                    <label htmlFor="ru">Русский</label>
                                </div>
                            </div>

                            {/* Български */}
                            <div className="row my-3">
                                <div className="col-2 pt-1">
                                    <input id="bg" className="form-check-input" name="lang" value="bg" type="radio"
                                      checked={this.props.config.lang === "bg"} onChange={this.changedLang}
                                    />
                                </div>
                                <div className="col-3">
                                    <label htmlFor="bg" className="flag"><FlagBg /></label>
                                </div>
                                <div className="col-7 pt-1">
                                    <label htmlFor="bg">Български</label>
                                </div>
                            </div>
                        </div>
                    </div>}
                </div>
            </div>

            <SaveRestart config={this.props.config} save="1" restart="1" />
        </>);
    };
};

export default Language;