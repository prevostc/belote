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

export const Card = enhance(({ color, motif }) => {
    return (
        <span>{colorMap[color]} {motif}</span>
    );
});

export default Card;

