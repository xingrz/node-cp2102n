import binding, { Interface } from './binding';

const CP2102N_PORTS = 7;

class CP2102N {
  private readonly intf: Interface;

  constructor(intf: Interface) {
    this.intf = intf;
  }

  async getRaw(): Promise<number> {
    return await this.intf.get();
  }

  async get(): Promise<Record<number, boolean>> {
    const read_bits = await this.intf.get();
    const result: Record<number, boolean> = {};
    for (let i = 0; i < CP2102N_PORTS; i++) {
      result[i] = (read_bits & (1 << i)) != 0;
    }
    return result;
  }

  async setRaw(state: number, mask: number): Promise<number> {
    return await this.intf.set(state, mask);
  }

  async set(state: Record<number, boolean>): Promise<Record<number, boolean>> {
    let write_bits = 0, write_mask = 0;
    for (let i = 0; i < CP2102N_PORTS; i++) {
      if (typeof state[i] != 'undefined') {
        write_bits |= state[i] ? (1 << i) : 0;
        write_mask |= (1 << i);
      }
    }
    const read_bits = await this.intf.set(write_bits, write_mask);
    const result: Record<number, boolean> = {};
    for (let i = 0; i < CP2102N_PORTS; i++) {
      result[i] = (read_bits & (1 << i)) != 0;
    }
    return result;
  }

  async getSerialNumber(): Promise<string> {
    return await this.intf.getSerialNumber();
  }

  close(): void {
    this.intf.close();
  }
}

export async function openInterface(path: string): Promise<CP2102N> {
  return new CP2102N(await binding.openInterface(path));
}
