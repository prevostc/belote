import React from 'react';
import {compose, pure} from "recompose";
import CardBack from './Card_back.svg';

const enhance = compose(
    pure
);

export const HiddenCard = enhance((props) => {
    return <img {...props} alt="a face down card" src={CardBack} />
});

