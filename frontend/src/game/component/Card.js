import React from 'react';
import {compose, pure} from "recompose";
import {AspectRatio} from "./AspectRatio";
import {Color} from "./Color";
import './Card.css';

const enhance = compose(
    pure
);

const motifMap = {
    "1": 1,
    "7": 7,
    "8": 8,
    "9": 9,
    "10": 10,
};

const motifTotxt = (motif) => {
    const mapped = motifMap[motif];
    if (mapped === 1) return 'A';
    else if (mapped === 11) return 'J';
    else if (mapped === 12) return 'Q';
    else if (mapped === 13) return 'K';
    else return `${mapped}`;
};

export const Card = enhance(({ color, motif }) => {
    return (
        // Poker size is 63.5mm X 88.9mm
        <AspectRatio className={`card`} ratio={63.5/88.9}>
            <div className={`card__layout`}>
                <div className={`card__layout__left-motif`}>{motifTotxt(motif)}</div>
                <div className={`card__layout__left-color`}><Color color={color}/></div>
                <div className={`card__layout__right-motif`}>{motifTotxt(motif)}</div>
                <div className={`card__layout__right-color`}><Color color={color}/></div>

                <div className={`card__layout__inner`}>
                    <div className={`card__inner card__inner--${motif}`}>
                    {[...Array(motifMap[motif])].map((_, i) => {
                        const style = { gridArea: `v${i+1}` };
                        return <span key={i} className="card__motif" style={style}><Color color={color}/></span>
                    })}
                    </div>
                </div>
            </div>
        </AspectRatio>
    );
});

export default Card;

