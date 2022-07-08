import { join, resolve } from 'path';

export interface Interface {
  get(): Promise<number>;
  set(state: number, mask: number): Promise<number>;
  getSerialNumber(): Promise<string>;
  close(): void;
}

export interface Binding {
  openInterface(path: string): Promise<Interface>;
}

export default ((name: string): Binding => {
  const moduleRoot = resolve(__dirname, '..');
  const paths = [
    join(moduleRoot, 'build', 'Release', name),
    join(moduleRoot, 'prebuilds', `${process.platform}-${process.arch}`, name)
  ];

  for (const path of paths) {
    try {
      return require(path);
    } catch (e) {
    }
  }

  throw new Error('Failed loading addon');
})('cp2102n.node');
