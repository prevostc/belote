import React from 'react';

const players = [
    { uuid: "1", name: "north"},
    { uuid: "2", name: "east"},
    { uuid: "3", name: "south"},
    { uuid: "4", name: "west"},
];

export const DebugPlayerSwitch = ({ setPlayer }) => {
    return (
        <div>
            {players.map(p => <button key={p.uuid} onClick={() => setPlayer(p)}>Become
                player {p.name} {p.uuid}</button>)}
        </div>
    );
};

export default DebugPlayerSwitch;
