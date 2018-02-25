import React from 'react';
import { compose, pure } from "recompose";
import {Color} from './component/Color';
import {enhanceWithGame} from "../api";

const enhance = compose(
    enhanceWithGame((game) => ({
        contract: game.contract,
    })),
    pure
);

export const Contract = enhance(({ contract }) => {
    return (<div>
        <h2>contract</h2>
        {contract === null
        ? (<span></span>)
        : (<div><Color color={contract.trump}/> - {contract.value} - {contract.player.spot} - {contract.player.team}</div>)}
    </div>);
});

export default Contract;
