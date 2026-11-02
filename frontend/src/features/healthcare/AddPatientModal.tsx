import { useState, type SubmitEvent } from 'react';
import { Button, Col, Form, Modal, Row } from 'react-bootstrap';

interface AddPatientModalProps {
    show: boolean;
    onHide: () => void;
    onSubmit: (name: string, age: number, diagnosis: string) => Promise<boolean>;
}

function AddPatientModal({ show, onHide, onSubmit }: AddPatientModalProps) {
    const [name, setName] = useState('');
    const [age, setAge] = useState('');
    const [diagnosis, setDiagnosis] = useState('');
    const [busy, setBusy] = useState(false);

    async function handleSubmit(event: SubmitEvent<HTMLFormElement>) {
        event.preventDefault();
        const numericAge = Number(age);
        if (!Number.isFinite(numericAge) || numericAge < 0) {
            return;
        }
        setBusy(true);
        const ok = await onSubmit(name.trim(), numericAge, diagnosis.trim());
        setBusy(false);
        if (ok) {
            setName('');
            setAge('');
            setDiagnosis('');
            onHide();
        }
    }

    return (
        <Modal show={show} onHide={onHide} centered>
            <Form onSubmit={handleSubmit}>
                <Modal.Header closeButton><Modal.Title>Add patient</Modal.Title></Modal.Header>
                <Modal.Body>
                    <Row className="g-3">
                        <Col sm={8}>
                            <Form.Label htmlFor="patient-name">Patient name</Form.Label>
                            <Form.Control
                                id="patient-name"
                                value={name}
                                onChange={(event) => setName(event.target.value)}
                                maxLength={80}
                                required
                            />
                        </Col>
                        <Col sm={4}>
                            <Form.Label htmlFor="patient-age">Age</Form.Label>
                            <Form.Control
                                id="patient-age"
                                type="number"
                                min={0}
                                step={1}
                                value={age}
                                onChange={(event) => setAge(event.target.value)}
                                required
                            />
                        </Col>
                        <Col xs={12}>
                            <Form.Label htmlFor="patient-diagnosis">Working diagnosis (optional)</Form.Label>
                            <Form.Control
                                id="patient-diagnosis"
                                value={diagnosis}
                                onChange={(event) => setDiagnosis(event.target.value)}
                                maxLength={120}
                            />
                        </Col>
                    </Row>
                </Modal.Body>
                <Modal.Footer>
                    <Button variant="outline-secondary" onClick={onHide} disabled={busy}>
                        Cancel
                    </Button>
                    <Button
                        type="submit"
                        className="healthcare-primary-button"
                        disabled={busy || !name.trim() || !age}
                    >
                        {busy ? 'Adding…' : 'Add patient'}
                    </Button>
                </Modal.Footer>
            </Form>
        </Modal>
    );
}

export default AddPatientModal;