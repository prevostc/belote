import React from 'react';
import CreateGame from './CreateGame';
import JoinGame from './JoinGame';
import {compose, pure} from "recompose";

const enhance = compose(
    pure
);

const Menu = enhance(() => {
    return (
        <div>
            <CreateGame onGameCreated={(uuid) => window.location.replace(`/game/${uuid}`)}/>
            <JoinGame onGameJoined={() => {}}/>
        </div>
    );
});

export default Menu