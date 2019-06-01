import React from 'react';
import {compose, lifecycle, mapProps, withStateHandlers} from "recompose";
import omit from "lodash/fp/omit";
import debounce from "lodash/debounce";
import './AspectRatio.css';

const elToState = (element, ratio) => {
    const clientWidth = element.parentNode.clientWidth;
    const clientHeight = element.parentNode.clientHeight;
    if (clientWidth / clientHeight > ratio) {
        return {
            height: clientHeight,
            width: clientHeight * ratio,
        }
    } else if (clientWidth / clientHeight < ratio) {
        return {
            height: clientWidth / ratio,
            width: clientWidth,
        }
    }
    return {
        height: clientHeight,
        width: clientWidth,
    }
};

const enhance = compose(
    withStateHandlers(
        {
            width: 1,
            height: 1,
            ref: null,
        },
        {
            gotRef: (state, {ratio}) => (ref) => ({ ref, ...elToState(ref, ratio) }),
            onResize: ({ref}, {ratio}) => (event) => ({ ref, ...elToState(ref, ratio) }),
        }
    ),
    lifecycle({
        componentWillMount() {
            this.debounce = debounce(this.props.onResize);
            window.addEventListener('resize', this.debounce);
        },
        componentWillUnmount() {
            window.removeEventListener('resize', this.debounce);
        }
    }),
    mapProps(props => omit(['onResize'], props))
);

export const AspectRatio = enhance(({ratio, width, height, gotRef, children, ...props}) => {
    const propStyle = props.style || {};
    return (
        <div {...props} ref={gotRef} style={{ height, width, "--aspect-ratio": ratio, ...propStyle }}>
            {children}
        </div>
    );
});
