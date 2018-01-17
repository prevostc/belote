import React from 'react';
import CreateGame from './CreateGame';
import JoinGameWithUuid from './JoinGameWithUuid';
import {compose, pure} from "recompose";

const enhance = compose(
    pure
);

const Menu = enhance(() => {
    return (
        <div>
            <CreateGame onGameCreated={(uuid) => window.location.replace(`/game/${uuid}`)}/>
            <JoinGameWithUuid />
        </div>
    );
});

export default Menu