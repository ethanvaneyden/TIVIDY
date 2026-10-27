import { useState, type SubmitEvent } from 'react';
import { Button, Form, Modal } from 'react-bootstrap';

interface CreateInstanceModalProps {
    show: boolean;
    onHide: () => void;
}

function CreateInstanceModal({ show, onHide }: CreateInstanceModalProps) {
    const [name, setName] = useState('');

    function handleSubmit(event: SubmitEvent<HTMLFormElement>) {
        event.preventDefault();
    }

    return (
        <Modal show={show} onHide={onHide} centered>
            <Form onSubmit={handleSubmit}>
                <Modal.Header closeButton><Modal.Title>Create instance</Modal.Title></Modal.Header>
                <Modal.Body>
                    <Form.Label htmlFor="instance-name">Instance name</Form.Label>
                    <Form.Control id="instance-name" value={name} onChange={(event) => setName(event.target.value)} maxLength={60} required />
                    <p className="action-placeholder-note">This form is not connected yet. Nothing will be created.</p>
                </Modal.Body>
                <Modal.Footer>
                    <Button variant="outline-secondary" onClick={onHide}>Cancel</Button>
                    <Button type="submit" className="workflow-primary-button">Create instance</Button>
                </Modal.Footer>
            </Form>
        </Modal>
    );
}

export default CreateInstanceModal;
