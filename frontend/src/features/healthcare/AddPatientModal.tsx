import { useState, type SubmitEvent } from 'react';
import { Button, Col, Form, Modal, Row } from 'react-bootstrap';
import type { NewPatient } from '../../api/workflow';

interface AddPatientModalProps {
    show: boolean;
    busy: boolean;
    wardName?: string;
    onHide: () => void;
    onSubmit: (patient: NewPatient) => Promise<boolean>;
}

function AddPatientModal({ show, busy, wardName, onHide, onSubmit }: AddPatientModalProps) {
    const [name, setName] = useState('');
    const [age, setAge] = useState('');
    const [diagnosis, setDiagnosis] = useState('');
    const ageValue = Number(age);
    const ageValid = age !== '' && Number.isInteger(ageValue) && ageValue >= 0 && ageValue <= 130;
    const canSubmit = name.trim() !== '' && ageValid;

    async function handleSubmit(event: SubmitEvent<HTMLFormElement>) {
        event.preventDefault();
        if (!canSubmit) return;
        if (await onSubmit({ name: name.trim(), age: ageValue, diagnosis: diagnosis.trim() })) {
            setName('');
            setAge('');
            setDiagnosis('');
            onHide();
        }
    }

    return (
        <Modal show={show} onHide={onHide} centered>
            <Form onSubmit={handleSubmit}>
                <Modal.Header closeButton>
                    <Modal.Title>Add patient{wardName ? ` to ${wardName}` : ''}</Modal.Title>
                </Modal.Header>
                <Modal.Body>
                    <Row className="g-3">
                        <Col sm={8}>
                            <Form.Label htmlFor="patient-name">Patient name</Form.Label>
                            <Form.Control id="patient-name" value={name} onChange={(event) => setName(event.target.value)} maxLength={80} required autoFocus />
                        </Col>
                        <Col sm={4}>
                            <Form.Label htmlFor="patient-age">Age</Form.Label>
                            <Form.Control id="patient-age" type="number" min={0} max={130} step={1} value={age} onChange={(event) => setAge(event.target.value)} isInvalid={age !== '' && !ageValid} required />
                        </Col>
                        <Col xs={12}>
                            <Form.Label htmlFor="patient-diagnosis">Working diagnosis</Form.Label>
                            <Form.Control id="patient-diagnosis" value={diagnosis} onChange={(event) => setDiagnosis(event.target.value)} maxLength={120} />
                        </Col>
                    </Row>
                </Modal.Body>
                <Modal.Footer>
                    <Button variant="outline-secondary" onClick={onHide} disabled={busy}>Cancel</Button>
                    <Button type="submit" className="healthcare-primary-button" disabled={busy || !canSubmit}>
                        {busy ? 'Adding...' : 'Add patient'}
                    </Button>
                </Modal.Footer>
            </Form>
        </Modal>
    );
}

export default AddPatientModal;
