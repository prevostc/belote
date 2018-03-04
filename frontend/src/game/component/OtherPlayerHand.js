import React from 'react';
import {HiddenCard} from './HiddenCard';
import './OtherPlayerHand.css';

const cardGridSpan = 3;
const maxCards = 8;

export const OtherPlayerHand = ({ cardCount, vertical=false }) => {
    const offset = 1 /* grid indexes starts at 1 */ + Math.floor((maxCards - cardCount) / 2);
    return <div className={`other-player__hand ${ vertical ? 'other-player__hand--vertical' : ''}`}>
        {[...Array(cardCount)].map((_, i) => {
            const cardStyle = vertical
                ? { gridRowStart: i+offset, gridRowEnd: i+offset+cardGridSpan }
                : { gridColumnStart: i+offset, gridColumnEnd: i+offset+cardGridSpan };
            return <HiddenCard style={cardStyle} key={i} className={`other-player__hand__card ${ vertical ? 'other-player__hand__card--vertical' : ''}`} />
        })}
    </div>;
};