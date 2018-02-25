import React from 'react';
import {compose, pure} from "recompose";

const enhance = compose(
    pure
);

const colorMap = {
    "SPADES": "♠",
    "HEARTS": "♥",
    "CLUBS": "♣",
    "DIAMONDS": "♦",
};

export const Color = enhance(({ color }) => {
    return (
        <span>{colorMap[color]}</span>
    );
});

export default Color;

