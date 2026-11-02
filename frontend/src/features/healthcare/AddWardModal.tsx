import { useState, type SubmitEvent } from 'react';
import { Button, Form, Modal } from 'react-bootstrap';

interface AddWardModalProps {
    show: boolean;
    onHide: () => void;
    onSubmit: (name: string) => Promise<boolean>;
}

// function AddWardModal({ show, onHide }: AddWardModalProps) {
//     const [name, setName] = useState('');

function AddWardModal({ show, onHide, onSubmit }: AddWardModalProps) {
    const [name, setName] = useState('');
    const [busy, setBusy] = useState(false);

    async function handleSubmit(event: SubmitEvent<HTMLFormElement>) {
        event.preventDefault();
        setBusy(true);
        const ok = await onSubmit(name.trim());
        setBusy(false);
        if (ok) {
            setName('');
            onHide();
        }
    }

    return (
        <Modal show={show} onHide={onHide} centered>
            <Form onSubmit={handleSubmit}>
                <Modal.Header closeButton><Modal.Title>Add ward</Modal.Title></Modal.Header>
                <Modal.Body>
                    <Form.Label htmlFor="ward-name">Ward name</Form.Label>
                    <Form.Control
                        id="ward-name"
                        value={name}
                        onChange={(event) => setName(event.target.value)}
                        maxLength={80}
                        required
                    />
                </Modal.Body>
                <Modal.Footer>
                    <Button variant="outline-secondary" onClick={onHide} disabled={busy}>
                        Cancel
                    </Button>
                    <Button
                        type="submit"
                        className="healthcare-primary-button"
                        disabled={busy || !name.trim()}
                    >
                        {busy ? 'Adding…' : 'Add ward'}
                    </Button>
                </Modal.Footer>
            </Form>
        </Modal>
    );
}

export default AddWardModal;