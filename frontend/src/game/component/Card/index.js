import React from 'react';
import PropTypes from 'prop-types';
import {compose, pure, setPropTypes} from "recompose";
import {validColors} from "../Color";
import * as svgs from "./assets";
import './Card.css';

const motifMap = {
    "ACE": "ace",
    "1": "ace",
    "7": "7",
    "V7": "7",
    "8": "8",
    "V8": "8",
    "9": "9",
    "V9": "9",
    "10": "10",
    "V10": "10",
    "JACK": "jack",
    "QUEEN": "queen",
    "KING": "king",
};

const colorMap = {
    "SPADES": "spades",
    "HEARTS": "hearts",
    "CLUBS": "clubs",
    "DIAMONDS": "diamonds",
    "♠": "spades",
    "♥": "hearts",
    "♣": "clubs",
    "♦": "diamonds",
};
const enhance = compose(
    setPropTypes({
        color: PropTypes.oneOf(validColors).isRequired,
        motif: PropTypes.oneOf(Object.keys(motifMap)).isRequired,
    }),
    pure
);

export const Card = enhance(({ color, motif, ...props }) => {
    const propClassName = props.className || {};
    const svg = svgs[`svg_${motifMap[motif]}_of_${colorMap[color]}${['king', 'queen', 'jack'].includes(motifMap[motif]) ? '2' : ''}`];
    return (
        <div {...props} className={`card ${propClassName}`}>
            <div className={`card__border`}>
                <img className={`card__img`} alt={`${motif} ${color}`} src={svg} />
            </div>
        </div>
    );
});

export default Card;

