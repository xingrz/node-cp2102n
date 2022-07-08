import load from 'bindings';

export interface Interface {
  get(): Promise<number>;
  set(state: number, mask: number): Promise<number>;
  getSerialNumber(): Promise<string>;
  close(): void;
}

export interface Binding {
  openInterface(path: string): Promise<Interface>;
}

export default <Binding>load('cp2102n');
