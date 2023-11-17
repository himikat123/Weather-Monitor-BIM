import React from "react";
import { Link, useLocation } from 'react-router-dom';
import Languages from "../Languages";
import {ReactComponent as DisplayImage} from "../svg/display.svg";

function MenuDisplay(props) {
    let text = new Languages(), lang = props.language;
    let location = useLocation().pathname;
    let isDisplay = '';
    if(location == '/display') isDisplay = ' active-menu-item';

    return (
        <li className="nav-item">
            <Link to={'/display'} className="nav-link active d-flex" title={text.get('display', lang)}>
                <div className={"menu-icon" + isDisplay}>
                    <DisplayImage />
                </div>
                <div className="d-lg-none ms-3 mt-1 text-light">{text.get('display', lang)}</div>
            </Link>
        </li>
    );
};

export default MenuDisplay;