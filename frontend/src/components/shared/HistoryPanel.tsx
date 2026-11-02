interface HistoryPanelProps {
    entries: string[];
}

/** Visual weight of a history line, so exceptions stand out in the audit trail. */
function kindOf(entry: string): string {
    if (entry.startsWith('Denied:')) return 'history-denied';
    if (entry.startsWith('[System]')) return 'history-system';
    if (/rejected|could not be approved|No (marker|doctor) available/i.test(entry)) return 'history-warning';
    return '';
}

function HistoryPanel({ entries }: HistoryPanelProps) {
    return (
        <section className="history-panel" aria-label="Workflow history">
            <div className="history-panel-header">
                <span className="workflow-section-label">HISTORY &amp; AUDIT TRAIL</span>
                <span className="history-count">{entries.length} events</span>
            </div>
            {entries.length > 0 ? (
                <ol className="history-list">
                    {entries.map((entry, index) => (
                        <li key={`${index}-${entry}`} className={kindOf(entry)}>{entry}</li>
                    ))}
                </ol>
            ) : (
                <p className="history-empty">Nothing has happened in this workflow yet.</p>
            )}
        </section>
    );
}

export default HistoryPanel;
