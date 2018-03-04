import React from 'react';
import './Hand.css';

const cardGridSpan = 3;
const maxCards = 8;

const isVertical = (orientation) => orientation === 'left' || orientation === 'right';

export const Hand = ({ cards, orientation, children }) => {
    const cardCount = cards.length;
    const offset = 1 /* grid indexes starts at 1 */ + Math.floor((maxCards - cardCount) / 2);
    return <div className={`hand hand--${orientation}`}>
        {cards.map((card, i) => {
            const cardStyle = isVertical(orientation)
                ? { gridRowStart: i+offset, gridRowEnd: i+offset+cardGridSpan }
                : { gridColumnStart: i+offset, gridColumnEnd: i+offset+cardGridSpan };
            return children(card, {style: cardStyle, key: i, className: `hand__card hand__card--${orientation}`});
        })}
    </div>;
};