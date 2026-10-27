import { useState, type SubmitEvent } from 'react';
import { Button, Form, Modal } from 'react-bootstrap';

interface AddSubmissionModalProps {
    show: boolean;
    onHide: () => void;
}

function AddSubmissionModal({ show, onHide }: AddSubmissionModalProps) {
    const [studentName, setStudentName] = useState('');

    function handleSubmit(event: SubmitEvent<HTMLFormElement>) {
        event.preventDefault();
    }

    return (
        <Modal show={show} onHide={onHide} centered>
            <Form onSubmit={handleSubmit}>
                <Modal.Header closeButton><Modal.Title>Add submission</Modal.Title></Modal.Header>
                <Modal.Body>
                    <Form.Label htmlFor="student-name">Student name</Form.Label>
                    <Form.Control id="student-name" value={studentName} onChange={(event) => setStudentName(event.target.value)} maxLength={80} required />
                    <p className="action-placeholder-note">This form is not connected yet. Nothing will be added.</p>
                </Modal.Body>
                <Modal.Footer>
                    <Button variant="outline-secondary" onClick={onHide}>Cancel</Button>
                    <Button type="submit" className="workflow-primary-button">Add submission</Button>
                </Modal.Footer>
            </Form>
        </Modal>
    );
}

export default AddSubmissionModal;
