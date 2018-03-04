import React from 'react';
import {compose, pure} from "recompose";
import CardBack from './HiddenCard.svg';

const enhance = compose(
    pure
);

export const HiddenCard = enhance((props) => {
    return <img {...props} alt="a face down card" src={CardBack} />
});

