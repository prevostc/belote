import React from 'react';
import {compose, pure} from "recompose";
import Color from './Color';

const enhance = compose(
    pure
);

export const Card = enhance(({ color, motif }) => {
    return (
        <span><Color color={color}/> {motif}</span>
    );
});

export default Card;

