
import { Container, Row, Col } from "react-bootstrap";
import "./EducationInstanceSelect.css";

interface EducationInstanceSelectProps {
    ids: string[];
}

function EducationInstanceSelect({ ids }: EducationInstanceSelectProps) {
    return (
        <Container className="py-5">
            <div className="mb-5">
                <p className="text-primary fw-semibold text-uppercase small mb-2">
                    Assignment Marking
                </p>

                <h1 className="landing-header mb-3">
                    Choose an instance
                </h1>

                <p className="text-secondary mb-0">
                    Select an instance to continue.
                </p>
            </div>

            <Row className="g-3">
                {ids.map((id) => (
                    <Col key={id} xs={12} md={6} lg={4}>
                        <div className="instance-wrapper">
                            <button
                                type="button"
                                className="instance-card w-100 d-flex align-items-center gap-3 p-4 text-start">
                                <h2 className="h5 mb-1">{id}</h2>
                            </button>

                            <button
                                type="button"
                                className="instance-delete"
                                aria-label={`Delete instance ${id}`}
                            >
                                ×
                            </button>
                        </div>
                    </Col>
                ))}
            </Row>

            <Row className="mt-4">
                <Col>
                    <button type="button" className="cta-button">
                        + Create Instance
                    </button>
                </Col>
            </Row>
        </Container>
    );
}

export default EducationInstanceSelect;
