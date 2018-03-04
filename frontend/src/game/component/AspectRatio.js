import React from 'react';
import {compose, pure} from "recompose";
import './AspectRatio.css';

const enhance = compose(
    pure
);

export const AspectRatio = enhance(({ratio, children, ...props}) => {
    const style = props.style
        ? {...props.style, "--aspect-ratio": ratio}
        : {"--aspect-ratio": ratio};
    return (
        <div {...props} style={style}>
            <div>
                {children}
            </div>
        </div>
    );
});
