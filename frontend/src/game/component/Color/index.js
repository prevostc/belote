import React from 'react';
import {compose, pure, setPropTypes} from "recompose";
import './Color.css';
import Club from "./ColorClub.svg";
import Diamond from "./ColorDiamond.svg";
import Heart from "./ColorHeart.svg";
import Spade from "./ColorSpade.svg";
import PropTypes from "prop-types";

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

export const validColors = Object.keys(colorMap);

const enhance = compose(
    setPropTypes({
        color: PropTypes.oneOf(validColors).isRequired,
    }),
    pure
);

export const Color = enhance(({ color }) => {
    return (
        <img className={`color`} alt={color} src={colorMap[color]} />
    );
});

export default Color;

