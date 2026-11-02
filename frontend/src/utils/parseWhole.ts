export function parseWhole(value: string, min: number, max: number): number | undefined {
    if (!/^\d{1,3}$/.test(value.trim())) return undefined;
    const parsed = Number(value);
    return parsed >= min && parsed <= max ? parsed : undefined;
}
