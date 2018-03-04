import React from 'react';
import {compose, pure} from "recompose";
import './Color.css';
import Club from "./ColorClub.svg";
import Diamond from "./ColorDiamond.svg";
import Heart from "./ColorHeart.svg";
import Spade from "./ColorSpade.svg";

const enhance = compose(
    pure
);

const colorMap = {
    "SPADES": Spade,
    "HEARTS": Heart,
    "CLUBS": Club,
    "DIAMONDS": Diamond,
    "♠": Spade,
    "♥": Heart,
    "♣": Club,
    "♦": Diamond,
};

export const Color = enhance(({ color }) => {
    return (
        <img className={`color`} alt={color} src={colorMap[color]} />
    );
});

export default Color;

