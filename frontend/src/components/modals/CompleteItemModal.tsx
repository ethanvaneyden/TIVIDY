import { useState, type SubmitEvent } from 'react';
import { Button, Form, Modal } from 'react-bootstrap';
import { parseWhole } from '../../utils/parseWhole';

interface CompleteItemModalProps {
    itemName?: string;
    title: string;
    label: string;
    min: number;
    max: number;
    hint: string;
    busy: boolean;
    primaryClassName?: string;
    onHide: () => void;
    onSubmit: (value: number) => Promise<boolean>;
}

function CompleteItemModal({ itemName, title, label, min, max, hint, busy, primaryClassName = 'workflow-primary-button', onHide, onSubmit }: CompleteItemModalProps) {
    const [text, setText] = useState('');
    const value = parseWhole(text, min, max);
    const invalid = text !== '' && value === undefined;

    async function handleSubmit(event: SubmitEvent<HTMLFormElement>) {
        event.preventDefault();
        if (value === undefined) return;
        if (await onSubmit(value)) {
            setText('');
            onHide();
        }
    }

    return (
        <Modal show={itemName !== undefined} onHide={onHide} centered>
            <Form onSubmit={handleSubmit} noValidate>
                <Modal.Header closeButton><Modal.Title>{title}</Modal.Title></Modal.Header>
                <Modal.Body>
                    <p className="mb-3">For <strong>{itemName}</strong></p>
                    <Form.Label htmlFor="complete-value">{label} ({min}-{max})</Form.Label>
                    <Form.Control
                        id="complete-value"
                        inputMode="numeric"
                        value={text}
                        onChange={(event) => setText(event.target.value)}
                        isInvalid={invalid}
                        autoFocus
                        required
                    />
                    <Form.Control.Feedback type="invalid">
                        Enter a whole number from {min} to {max}.
                    </Form.Control.Feedback>
                    <p className="modal-hint">{hint}</p>
                </Modal.Body>
                <Modal.Footer>
                    <Button variant="outline-secondary" onClick={onHide} disabled={busy}>Cancel</Button>
                    <Button type="submit" className={primaryClassName} disabled={busy || value === undefined}>
                        {busy ? 'Saving...' : 'Complete'}
                    </Button>
                </Modal.Footer>
            </Form>
        </Modal>
    );
}

export default CompleteItemModal;
